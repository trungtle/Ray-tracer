#include "integrators/integrator.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <tbb/tbb.h>

#include "camera/camera.h"
#include "materials/material.h"
#include "ray/ray.h"
#include "samplers/pdf.h"
#include "samplers/sampler.h"
#include "shapes/scene.h"
#include "screen/screen.h"


#define PARALLEL 1
#define REPORT 1

vec3 deNaN(const vec3& v)
{
	vec3 temp = v;
	if (!(temp.x == temp.x)) temp.x = 0;
	if (!(temp.y == temp.y)) temp.y = 0;
	if (!(temp.z == temp.z)) temp.z = 0;
	return temp;
}

/** Sum over the number of samples
 */
class SumColor
{
	const Integrator* m_integrator;
	const Scene* m_scene;
    Ray m_ray;

public:

	void operator()(const tbb::blocked_range<size_t>& range)
	{
         Spectrum sum = _sumColor;
         size_t end = range.end();

         uint32_t depth = 50;
         for (size_t i = range.begin(); i != end; i++)
         {
             Spectrum li = m_integrator->Li(*m_scene, m_ray, depth);
             li.DeNaN();
             sum += li;
         }
        
         _sumColor = sum;
	}

	SumColor(SumColor& other, tbb::split) : 
        m_integrator(other.m_integrator), m_scene(other.m_scene), m_ray(other.m_ray), _sumColor(0.0)
	{}

	void join(const SumColor& other)
	{
		_sumColor += other._sumColor;
	}

	SumColor(
		const Integrator* integrator,
		const Scene* scene,
        const Ray& ray
		) :
			m_integrator(integrator),
			m_scene(scene),
            m_ray(ray),
			_sumColor(0.0)
	{}

	Spectrum _sumColor;
};


void Integrator::Render(const Scene& scene, uint32_t numSamplesPerPixel)
{
#if REPORT
    auto start = chrono::steady_clock::now();
#endif
    
#if PARALLEL

	parallel_for(tbb::blocked_range2d<unsigned int>(0, m_screen->height, 0, m_screen->width),
		[numSamplesPerPixel, this, &scene](tbb::blocked_range2d<unsigned int> range)
		{
			for (unsigned int y = range.rows().begin(); y < range.rows().end(); y++)
			{
				for (unsigned int x = range.cols().begin(); x < range.cols().end(); x++)
				{
                    vec2 uv = Sampler::RandomSampleFromPixel(x, y, m_screen->width, m_screen->height);
                    
                    Ray ray = m_camera->GetRay(uv);
                    
					SumColor sumColor(
						this,
                        &scene,
                        ray
                        );
					parallel_reduce(tbb::blocked_range<size_t>(0, numSamplesPerPixel), sumColor);
					Spectrum color = sumColor._sumColor;
					color /= float(numSamplesPerPixel);

					// Gamma correction
                    color = Sqrt(color);
                    m_camera->film->SetPixel(color, x, y);
				}
			}
		});
#else

	for (int y = m_screen->height - 1; y >= 0; y--)
	{
		for (int x = 0; x < m_screen->width; x++)
		{
			Spectrum color(0.0f);
			for (int n = 0; n < numSamplesPerPixel; n++)
			{
				vec2 uv = Sampler::RandomSampleFromPixel(x, y, m_screen->width, m_screen->height);
				Ray r = m_camera->GetRay(uv);
                Spectrum li = Li(scene, r, 50);
                li.DeNaN();
                color += li;
			}

			color /= float(numSamplesPerPixel);

			// Gamma correction
            color = Sqrt(color);
            m_camera->film->SetPixel(color, x, y);
		}
	}

#endif

#if REPORT
    auto end = chrono::steady_clock::now();
#endif

    // Output to file
    m_camera->film->WriteImage();
}


Spectrum Integrator::Li(const Scene& scene, const Ray& r, int maxDepth) const
{
    uint32_t depth = 0;
    Ray scatterRay = r;
    Spectrum accColor(0.0);
    while(depth < maxDepth)
    {
        Spectrum bounceColor(0.0);
        Intersection intersect;
        if (scene.Hit(scatterRay, 0.001, 10000.0f, intersect))
        {
            const Material* material = scene.materials[intersect.hit->materialId];
            Spectrum emitted = material->Emitted(intersect.UV, intersect.P);

            if (material->Scatter(r, intersect, scatterRay))
            {
                switch(material->type)
                {
                    case Material::kDiffuse:
                    {
                        Spectrum albedo = material->texture->value(intersect.UV, intersect.P);
                        Hitable* light = scene.lights[0];
                        HitablePDF pdfLight(light, intersect.P);
                        CosinePDF pdfCosine(intersect.N);
                        //UniformPDF pdfCosine(intersect.N);
                        //MixturePDF pdfMix(&pdfLight, &pdfCosine);
                        
                        // DEBUG
                        // HitablePDF pdfMix(light, intersect.P);
                        // CosinePDF pdfMix(intersect.N);
                        UniformPDF pdfMix(intersect.N);

                        vec3 scatteredDirection = pdfMix.Generate();
                        scatterRay = Ray(intersect.P, scatteredDirection, r.time);
                        float pdfVal = pdfMix.Value(scatterRay.direction);

                        float scatteringPdf = abs(dot(normalize(intersect.N), scatterRay.direction)) * INV_PI;
                        bounceColor = albedo * scatteringPdf / pdfVal;
                        //bounceColor = bounceColor.Clamp(0., 1.0f);
                        
                        if (depth == 0)
                        {
                            accColor = bounceColor;
                        }
                        else
                        {
                            accColor = accColor * bounceColor;
                        }

                        break;
                    }
                    case Material::kMetal:
                    {
                        Spectrum albedo = material->texture->value(intersect.UV, intersect.P);
                        bounceColor = emitted + albedo;
                        accColor *= albedo;
                        break;
                    }
                    case Material::kDielectric:
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            else
            {
                if (depth == 0)
                {
                    // Hit light source, just color it white
                    accColor = emitted.Clamp(0., 1.0f);
                    break;
                }
                else
                {
                    accColor *= emitted;
                    break;
                }
            }
        }
        else
        {
            if (depth == 0)
            {
                accColor = Screen::SkyColor(r);
            }
            else
            {
                accColor *= Screen::SkyColor(r);
                
            }
            
            break;
        }

        depth++;
    }
    
    return accColor;
}

#include "integrators/integrator.h"

#include <chrono>
#include <fstream>
#include <tbb/tbb.h>

#include "camera/camera.h"
#include "materials/material.h"
#include "ray/ray.h"
#include "samplers/pdf.h"
#include "samplers/sampler.h"
#include "shapes/scene.h"
#include "screen/screen.h"


#define PARALLEL 0
#define REPORT 1

vec3 deNaN(const vec3& v)
{
	vec3 temp = v;
	if (!(temp.x == temp.x)) temp.x = 0;
	if (!(temp.y == temp.y)) temp.y = 0;
	if (!(temp.z == temp.z)) temp.z = 0;
	return temp;
}

class SumColor
{
	int pixelX;
	int pixelY;
	std::shared_ptr<const Camera> m_camera;
	const Integrator* m_integrator;
	std::shared_ptr<const Screen> m_screen;
	const Scene* m_scene;

public:

	void operator()(const tbb::blocked_range<size_t>& range)
	{
		vec2 uv = Sampler::RandomSampleFromPixel(pixelX, pixelY, m_screen->width, m_screen->height);
		Ray ray = m_camera->GetRay(uv);

		vec3 sum = _sumColor;
		size_t end = range.end();		

		uint32_t depth = 50;
		for (size_t i = range.begin(); i != end; i++)
		{
			sum += m_integrator->Li(*m_scene, ray, depth);
		}
		_sumColor = sum;
	}

	SumColor(SumColor& other, tbb::split) : 
		pixelX(other.pixelX), pixelY(other.pixelY), _sumColor(vec3(0, 0, 0)) 
	{}

	void join(const SumColor& other)
	{
		_sumColor += other._sumColor;
	}

	SumColor(
		int x, 
		int y, 
		std::shared_ptr<const Camera> camera, 
		const Integrator* integrator,
		std::shared_ptr<const Screen> screen,
		const Scene* scene
		) :
			pixelX(x), 
			pixelY(y), 
			m_camera(camera),
			m_integrator(integrator),
			m_screen(screen),
			m_scene(scene),
			_sumColor(vec3(0, 0, 0))
	{}

	vec3 _sumColor;
};


void Integrator::Render(const Scene& scene, uint32_t numSamplesPerPixel)
{
#if PARALLEL

	vec3 film[m_screen->width][m_screen->height];

#if REPORT
	auto start = chrono::steady_clock::now();
#endif

	parallel_for(tbb::blocked_range2d<int>(0, m_screen->height, 0, m_screen->width),
		[numSamplesPerPixel, &film, this, &scene](tbb::blocked_range2d<int> range)
		{
			int rowEnd = range.rows().end();
			for (int y = range.rows().begin(); y < rowEnd; y++)
			{
				int colEnd = range.cols().end();
				for (int x = range.cols().begin(); x < colEnd; x++)
				{
					SumColor sumColor(
						x, 
						y, 
						m_camera, 
						this,
						m_screen,
						&scene);
					parallel_reduce(tbb::blocked_range<size_t>(0, numSamplesPerPixel), sumColor);
					vec3 color = sumColor._sumColor;
					color /= float(numSamplesPerPixel);

					// Gamma correction
					color = glm::sqrt(color);
					film[x][y] = color;

				}
			}
		});

#if REPORT
		auto end = chrono::steady_clock::now();
#endif	

		// Output to file

		ofstream file;
		file.open("../images/image.ppm");	
		file << "P3\n" << m_screen->width << " " << m_screen->height << "\n255\n";

		for (int y = m_screen->height - 1; y >= 0; y--)
		{
			for (int x = 0; x < m_screen->width; x++)
			{
				vec3 color = film[x][y];
				int ir = int(color.r * 255.99);
				int ig = int(color.g * 255.99);
				int ib = int(color.b * 255.99);
				file << ir << " " << ig << " " << ib << "\n";
			}
		}

		file.close();

#else

	ofstream file;
	file.open("../images/image.ppm");	
	file << "P3\n" << m_screen->width << " " << m_screen->height << "\n255\n";

#if REPORT
	auto start = chrono::steady_clock::now();
#endif

	for (int y = m_screen->height - 1; y >= 0; y--)
	{
		for (int x = 0; x < m_screen->width; x++)
		{
			vec3 color(0.0f, 0.0f, 0.0f);
			for (int n = 0; n < numSamplesPerPixel; n++)
			{
				vec2 uv = Sampler::RandomSampleFromPixel(x, y, m_screen->width, m_screen->height);
				Ray r = m_camera->GetRay(uv);
				color += Li(scene, r, 50);
			}

			color /= float(numSamplesPerPixel);

			// Gamma correction
			color = glm::sqrt(color);

			int ir = int(color.r * 255.99);
			int ig = int(color.g * 255.99);
			int ib = int(color.b * 255.99);

			file << ir << " " << ig << " " << ib << "\n";
		}
	}

#if REPORT
	auto end = chrono::steady_clock::now();
#endif	

	file.close();

#endif			
}

glm::vec3 Integrator::Li(const Scene& scene, const Ray& r, int maxDepth) const
{
		uint32_t depth = 0;
		Ray scatterRay = r;
		vec3 accColor;
		while(depth < maxDepth)
		{
			vec3 color;
			Intersection intersect;
			if (scene.Hit(scatterRay, 0.001, 10000.0f, intersect))
			{
				const Material* material = scene.materials[intersect.hit->materialId];
				vec3 emitted = material->Emitted(intersect.UV, intersect.P);

				if (material->Scatter(r, intersect, scatterRay))
				{
					switch(material->type)
					{
						case Material::kDiffuse:
						{
							vec3 albedo = material->texture->value(intersect.UV, intersect.P);
							Hitable* light = scene.lights[0];
							// HitablePDF pdfLight(light, intersect.P);
							// CosinePDF pdfCosine(intersect.N);
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
							
							color = emitted + albedo * scatteringPdf / pdfVal;	

							if (depth == 0)
							{
								accColor = color;							
							}
							else
							{
								accColor = accColor * color;
							}

							break;
						}
						case Material::kMetal:
						{
							vec3 albedo = material->texture->value(intersect.UV, intersect.P);
							color = emitted + albedo;
							accColor *= albedo;
							break;
						}
						case Material::kDielectric:
						{
							break;
						}
						case Material::kLight:
						default:
						{
							accColor *= emitted;
							accColor = deNaN(accColor);
							return accColor;
							break;
						}
					}
				}
				else
				{
					accColor *= emitted;
					break;
				}				
			}
			else
			{
				accColor *= Screen::SkyColor(r);
				break;
			}	

			depth++;	
		}


		accColor = deNaN(accColor);
		return accColor;		
}

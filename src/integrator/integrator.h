#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "intersection/scene.h"
#include "material/material.h"
#include "ray/ray.h"
#include "sampler/pdf.h"

class Integrator
{
public:
	vec3 Shade(Scene& scene, const Ray& r, int maxDepth)
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
						accColor = Tr::deNaN(accColor);
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


	accColor = Tr::deNaN(accColor);
	return accColor;		
	}

};

#endif
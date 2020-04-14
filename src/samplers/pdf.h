#ifndef PDF_H
#define PDF_H

#include <glm/glm.hpp>

#include "samplers/sampler.h"
#include "shapes/Intersection.h"
#include "transform/onb.h"

using namespace glm;
using namespace std;

class PDF
{
public:
	virtual float Value(const vec3& direction) const = 0;
	virtual vec3 Generate() const = 0;

};

class UniformPDF : public PDF
{
public:
	UniformPDF(const vec3& w)
	{
		uvw.BuildFromW(w);
	}

	virtual float Value(const vec3& direction) const override
	{
		return INV_PI;

	}

	virtual vec3 Generate() const override
	{
		return uvw.Local(Sampler::RandomDirection());
	}

	ONB uvw;
};

class CosinePDF : public PDF
{
public:
	CosinePDF(const vec3& w)
	{
		uvw.BuildFromW(w);
	}

	virtual float Value(const vec3& direction) const override
	{
		float cosine = dot(normalize(direction), normalize(uvw.w()));
		if (cosine >= 0)
		{
			return cosine * INV_PI;
		}
		return 0;

	}

	virtual vec3 Generate() const override
	{
		return uvw.Local(Sampler::RandomCosineDirection());
	}

	ONB uvw;

};

// A PDF for casting a ray from origin toward the hitable
class HitablePDF : public PDF
{
public:
	HitablePDF(Hitable* h, const vec3& o) :
		hitable(h), origin(o)
	{

	}

	virtual float Value(const vec3& direction) const override
	{
		return hitable->PdfValue(origin, direction);
	}

	virtual vec3 Generate() const override
	{
		return hitable->Random(origin);
	}

	Hitable* hitable;
	vec3 origin;
};

class MixturePDF : public PDF
{
public:
	MixturePDF(PDF* p1, PDF* p2) :
		pdf1(p1), pdf2(p2){}
    
	virtual float Value(const vec3& direction) const override
	{
		return 0.5f * pdf1->Value(direction) + 0.5f * pdf2->Value(direction);
	}

	virtual vec3 Generate() const override
	{
		if (Sampler::Random01() < 0.5)
		{
			return pdf1->Generate();
		}
		else
		{
			return pdf2->Generate();
		}
	}
    
    float Weight(vec3 direction)
    {
        float pdf1Val = pdf1->Value(direction);
        float pdf2Val = pdf2->Value(direction);
        float weight = MixturePDF::PowerHeuristic(1, pdf1Val, 1, pdf2Val);
        return weight;
    }

    static float BalanceHeuristic(int nf, float fPdf, int ng, float gPdf)
    {
        return (nf * fPdf) / (nf * fPdf + ng * gPdf);
    }
    
    // Should have better result than using the balance heuristic
    static float PowerHeuristic(int nf, loat fPdf, int ng, float gPdf)
    {
        float f = nf * fPdf, g = g * gPdf;
        return (f * f) / (f * f + g * g);
    }
    
	PDF* pdf1;
	PDF* pdf2;
    
};

#endif

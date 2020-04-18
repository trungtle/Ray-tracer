#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

#include "math/constants.h"

enum class SpectrumType { Reflectance, Illuminant };

/** Spectrum representation as a number of samples
 */
template <int nSpectrumSamples>
class CoefficientSpectrum
{
public:
    
    static const int nSamples = nSpectrumSamples;
    
    CoefficientSpectrum(float value = 0.0f)
    {
        m_c.fill(value);
    }
    
    void Print()
    {
        std::cout << "[";
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            std::cout << m_c[i];
            if (i + 1 < nSpectrumSamples)
            {
                std::cout << ", ";
            }
        }
        std::cout << "]";
    }
    
    friend
    std::ostream& operator<<(std::ostream& os, const CoefficientSpectrum& obj)
    {
        os << "[";
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            os << obj.m_c[i];
            if (i + 1 < nSpectrumSamples)
            {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }
    
    CoefficientSpectrum operator-() const
    {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] = -m_c[i];
        }
        return ret;
    }
    
    CoefficientSpectrum& operator+=(const CoefficientSpectrum& s2)
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            m_c[i] += s2.m_c[i];
        }
        return *this;
    }
    
    CoefficientSpectrum operator+(const CoefficientSpectrum& s2) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] += s2.m_c[i];
        }
        return ret;
    }
    
    CoefficientSpectrum operator-(const CoefficientSpectrum& s2) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] -= s2.m_c[i];
        }
        return ret;
    }

    CoefficientSpectrum& operator-=(const CoefficientSpectrum& s2)
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            m_c[i] -= s2.m_c[i];
        }
        return *this;
    }

    CoefficientSpectrum operator/(const CoefficientSpectrum& s2) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] /= s2.m_c[i];
        }
        return ret;
    }

    CoefficientSpectrum& operator/=(const CoefficientSpectrum& s2)
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            m_c[i] /= s2.m_c[i];
        }
        return *this;
    }
    
    CoefficientSpectrum operator/(float v) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] /= v;
        }
        assert(!ret.HasNaNs());
        return ret;
    }

    CoefficientSpectrum& operator/=(float v)
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            m_c[i] /= v;
        }
        return *this;
    }

    CoefficientSpectrum operator*(const CoefficientSpectrum& s2) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] *= s2.m_c[i];
        }
        return ret;
    }

    CoefficientSpectrum& operator*=(const CoefficientSpectrum& s2)
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            m_c[i] *= s2.m_c[i];
        }
        return *this;
    }
    
    CoefficientSpectrum operator*(float v) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] *= v;
        }
        return ret;
    }
    
    CoefficientSpectrum& operator*=(float v)
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            m_c[i] *= v;
        }
        return *this;
    }

    friend inline
    CoefficientSpectrum operator*(float v, const CoefficientSpectrum& s)
    {
        return s * v;
    }
    
    bool operator==(const CoefficientSpectrum& s2) const
    {
        return m_c == s2.m_c;
    }

    bool operator!=(const CoefficientSpectrum& s2) const
    {
        return m_c != s2.m_c;
    }
    
    bool IsBlack() const
    {
        for (int i = 0; i < nSpectrumSamples; ++i)
        {
            if (m_c[i] != 0.) return false;
        }
        return true;
    }
    
    friend
    CoefficientSpectrum Sqrt(const CoefficientSpectrum& s)
    {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] = std::sqrt(s.m_c[i]);
        }
        return ret;
    }
    
    friend
    CoefficientSpectrum Pow(const CoefficientSpectrum& s, float exp)
    {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] = std::pow(s.m_c[i], exp);
        }
        return ret;
    }

    friend
    CoefficientSpectrum Exp(const CoefficientSpectrum& s)
    {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            ret.m_c[i] = std::exp(s.m_c[i]);
        }
        return ret;
    }
    
    CoefficientSpectrum Clamp(float low = 0, float high = INFINITY) const
    {
        CoefficientSpectrum ret;
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            if (m_c[i] < low) ret.m_c[i] = low;
            if (m_c[i] > high) ret.m_c[i] = high;
            ret.m_c[i] = m_c[i];
        }
        return ret;
    }
    
    bool HasNaNs() const
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            if (std::isnan(m_c[i])) return true;
        }
        return false;
    }
    
    void DeNaN()
    {
        for (int i = 0; i < nSpectrumSamples; i++)
        {
            if (!(m_c[i] == m_c[i])) m_c[i] = 0;
        }
    }
    
    float& operator[](int i)
    {
        return m_c[i];
    }
    
    float operator[](int i) const
    {
        return m_c[i];
    }

protected:
    /** The number of coefficient samples */
    std::array<float, nSpectrumSamples> m_c;
};


/** RGB spectrum, while is a limited color representation, and monitor-dependent,
 * is convenient as almost all rendering tools out there use this representation
 */
class RGBSpectrum : public CoefficientSpectrum<3>
{
public:
    RGBSpectrum(float v = 0.f) : CoefficientSpectrum<3>(v) {}
    RGBSpectrum(float r, float g, float b)
    {
        m_c[0] = r;
        m_c[1] = g;
        m_c[2] = b;
    }
    RGBSpectrum(const CoefficientSpectrum<3>& v) : CoefficientSpectrum<3>(v) {}
    RGBSpectrum(const RGBSpectrum& s, SpectrumType type = SpectrumType::Reflectance)
    {
        *this = s;
    }
    
    static RGBSpectrum FromRGB(const float rgb[3], SpectrumType type = SpectrumType::Reflectance)
    {
        RGBSpectrum s;
        s.m_c[0] = rgb[0];
        s.m_c[1] = rgb[1];
        s.m_c[2] = rgb[2];
        return s;
    }
    
    void ToRGB(float* rgb) const
    {
        rgb[0] = m_c[0];
        rgb[1] = m_c[1];
        rgb[2] = m_c[2];
    }
    
    const RGBSpectrum &ToRGBSpectrum() const
    {
        return *this;
    }
};


typedef RGBSpectrum Spectrum;

#endif

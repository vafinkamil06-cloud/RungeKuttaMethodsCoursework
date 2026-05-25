#pragma once

double norm(
    const std::vector<double>& vec1, 
    const std::vector<double>& vec2
)
{
    std::vector<double> diff(vec1.size());
    std::transform(
        vec1.begin(),
        vec1.end(),
        vec2.begin(),
        diff.begin(),
        [&](double val1, double val2) 
        { 
            return std::fabs(val1 - val2); 
        }
    );

    return std::accumulate(diff.begin(), diff.end(), 0.0);
}

std::vector<double> operator*(double scalar, std::vector<double> vec)
{
    std::vector<double> result(vec.size());
    std::transform(
        vec.begin(),
        vec.end(),
        result.begin(),
        [scalar](double value) 
        { 
            return value * scalar; 
        }
    );

    return result;
}


std::vector<double> operator+(const std::vector<double>& vec1, const std::vector<double>& vec2)
{
    std::vector<double> result(vec1.size());
    std::transform(
        vec1.begin(),
        vec1.end(),
        vec2.begin(),
        result.begin(),
        [&](double val1, double val2) 
        { 
            return val1 + val2; 
        }
    );

    return result;
}


std::vector<double> operator-(const std::vector<double>& vec1, const std::vector<double>& vec2)
{
    return vec1 + (-1) * vec2;
}


std::vector<double> operator+(const std::vector<double>& vec, double scalar)
{
    std::vector<double> result(vec.size());
    std::transform(
        vec.begin(),
        vec.end(),
        result.begin(),
        [&](double val) 
        { 
            return val + scalar; 
        }
    );

    return result;
}

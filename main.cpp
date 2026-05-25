#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "methods/utils/vector_operations.h"
#include "plot/matplotlibcpp.h"

namespace coursework
{

using value_type = double;
using vector_of_values = std::vector<value_type>;
using rhs_type = std::function<vector_of_values(value_type, const vector_of_values&)>;

struct solution
{
    std::vector<value_type> t;
    std::vector<vector_of_values> y;
};

vector_of_values merson_step(
    const rhs_type& f,
    value_type t,
    const vector_of_values& y,
    value_type h
)
{
    auto k1 = f(t, y);
    auto k2 = f(t + h / 3.0, y + h / 3.0 * k1);
    auto k3 = f(t + h / 3.0, y + h / 6.0 * k1 + h / 6.0 * k2);
    auto k4 = f(t + h / 2.0, y + h / 8.0 * k1 + 3.0 * h / 8.0 * k3);
    auto k5 = f(t + h, y + h / 2.0 * k1 - 3.0 * h / 2.0 * k3 + 2.0 * h * k4);

    return y + h / 6.0 * (k1 + 4.0 * k4 + k5);
}

solution integrate_merson(
    const rhs_type& f,
    value_type start_t,
    const vector_of_values& start_y,
    value_type end_t,
    value_type h
)
{
    solution result;
    result.t.push_back(start_t);
    result.y.push_back(start_y);

    value_type t = start_t;
    vector_of_values y = start_y;

    while (t < end_t - 1e-12)
    {
        value_type current_h = std::min(h, end_t - t);
        y = merson_step(f, t, y, current_h);
        t += current_h;

        result.t.push_back(t);
        result.y.push_back(y);
    }

    return result;
}

value_type max_norm_error(
    const solution& numeric,
    const std::function<vector_of_values(value_type)>& exact
)
{
    value_type max_error = 0.0;

    for (std::size_t i = 0; i < numeric.t.size(); ++i)
    {
        auto exact_y = exact(numeric.t[i]);
        value_type point_error = norm(numeric.y[i], exact_y);
        max_error = std::max(max_error, point_error);
    }

    return max_error;
}

void save_csv(const std::string& filename, const solution& data)
{
    std::ofstream out(filename);
    out << std::setprecision(17);
    out << "t";
    for (std::size_t i = 0; i < data.y.front().size(); ++i)
    {
        out << ",y" << i;
    }
    out << '\n';

    for (std::size_t i = 0; i < data.t.size(); ++i)
    {
        out << data.t[i];
        for (auto value : data.y[i])
        {
            out << ',' << value;
        }
        out << '\n';
    }
}

std::vector<value_type> component(const solution& data, std::size_t index)
{
    std::vector<value_type> values(data.y.size());
    std::transform(
        data.y.begin(),
        data.y.end(),
        values.begin(),
        [index](const vector_of_values& row) { return row[index]; }
    );
    return values;
}

rhs_type test_rhs()
{
    return [](value_type t, const vector_of_values& y)
    {
        value_type denominator = std::sqrt(1.0 + std::exp(2.0 * t));
        value_type radius_part = y[0] * y[0] + y[1] * y[1] - 1.0;

        return vector_of_values{
            -std::sin(t) / denominator + y[0] * radius_part,
             std::cos(t) / denominator + y[1] * radius_part
        };
    };
}

std::function<vector_of_values(value_type)> test_exact()
{
    return [](value_type t)
    {
        value_type denominator = std::sqrt(1.0 + std::exp(2.0 * t));
        return vector_of_values{
            std::cos(t) / denominator,
            std::sin(t) / denominator
        };
    };
}

void run_task2()
{
    namespace plt = matplotlibcpp;

    value_type start_t = 0.0;
    value_type end_t = 5.0;
    value_type h = 0.1;
    auto exact = test_exact();
    vector_of_values start_y = exact(start_t);

    auto numeric = integrate_merson(test_rhs(), start_t, start_y, end_t, h);

    std::vector<value_type> exact_y1;
    std::vector<value_type> exact_y2;
    exact_y1.reserve(numeric.t.size());
    exact_y2.reserve(numeric.t.size());

    for (auto t : numeric.t)
    {
        auto y = exact(t);
        exact_y1.push_back(y[0]);
        exact_y2.push_back(y[1]);
    }

    save_csv("task2_test_solution.csv", numeric);

    plt::figure();
    plt::named_plot("numeric y1", numeric.t, component(numeric, 0));
    plt::named_plot("exact y1", numeric.t, exact_y1, "--");
    plt::named_plot("numeric y2", numeric.t, component(numeric, 1));
    plt::named_plot("exact y2", numeric.t, exact_y2, "--");
    plt::title("Test problem solution");
    plt::xlabel("t");
    plt::ylabel("y");
    plt::grid(true);
    plt::legend();
    plt::save("task2_test_solution.png");
    plt::close();

    std::cout << "Task 2 max error: " << max_norm_error(numeric, exact) << '\n';
    std::cout << "Saved task2_test_solution.png and task2_test_solution.csv\n";
}

void run_task3()
{
    namespace plt = matplotlibcpp;

    value_type start_t = 0.0;
    value_type end_t = 5.0;
    auto exact = test_exact();
    vector_of_values start_y = exact(start_t);
    std::vector<value_type> steps = {0.03125, 0.0625, 0.125, 0.25, 0.5};
    std::vector<value_type> errors;
    std::vector<value_type> scaled_errors;

    std::ofstream out("task3_errors.csv");
    out << std::setprecision(17);
    out << "h,error,error_div_h5\n";
    std::cout << "Task 3 errors:\n";
    std::cout << "h\te(h)\te(h)/h^5\n";

    for (auto h : steps)
    {
        auto numeric = integrate_merson(test_rhs(), start_t, start_y, end_t, h);
        value_type error = max_norm_error(numeric, exact);
        value_type scaled_error = error / std::pow(h, 5.0);

        errors.push_back(error);
        scaled_errors.push_back(scaled_error);
        out << h << ',' << error << ',' << scaled_error << '\n';
        std::cout << h << '\t' << error << '\t' << scaled_error << '\n';
    }

    plt::figure_size(1500, 650);
    plt::subplot(1, 2, 1);
    plt::named_plot("max e", steps, errors, "o-");
    plt::title("Maximum error");
    plt::xlabel("h");
    plt::ylabel("max e");
    plt::tick_params({{"labelrotation", "35"}}, "x");
    plt::grid(true);
    plt::legend();

    plt::subplot(1, 2, 2);
    plt::named_plot("max e / h^5", steps, scaled_errors, "o-");
    plt::title("Scaled maximum error");
    plt::xlabel("h");
    plt::ylabel("max e / h^5");
    plt::tick_params({{"labelrotation", "35"}}, "x");
    plt::grid(true);
    plt::legend();
    plt::save("task3_errors.png");
    plt::close();

    std::cout << "Saved task3_errors.png and task3_errors.csv\n";
}

rhs_type arenstorf_rhs()
{
    return [](value_type, const vector_of_values& points)
    {
        value_type m = 0.012277471;
        value_type M = 1.0 - m;
        value_type x = points[0];
        value_type y = points[1];
        value_type u = points[2];
        value_type v = points[3];

        value_type r1 = std::pow((x + m) * (x + m) + y * y, 1.5);
        value_type r2 = std::pow((x - M) * (x - M) + y * y, 1.5);

        return vector_of_values{
            u,
            v,
            x + 2.0 * v - M * (x + m) / r1 - m * (x - M) / r2,
            y - 2.0 * u - M * y / r1 - m * y / r2
        };
    };
}

void run_task4()
{
    namespace plt = matplotlibcpp;

    value_type start_t = 0.0;
    value_type period = 11.124340337;
    value_type h = 0.001;
    vector_of_values start_y = {0.994, 0.0, 0.0, -2.031732629557337};

    auto numeric = integrate_merson(arenstorf_rhs(), start_t, start_y, period, h);
    save_csv("task4_arenstorf_orbit.csv", numeric);

    plt::figure();
    plt::named_plot("orbit", component(numeric, 0), component(numeric, 1));
    plt::plot({1.0}, {0.0}, {{"color", "red"}, {"marker", "o"}, {"linestyle", "None"}});
    plt::plot({0.0}, {0.0}, {{"color", "green"}, {"marker", "o"}, {"linestyle", "None"}});
    plt::title("Restricted three body system");
    plt::xlabel("x");
    plt::ylabel("y");
    plt::grid(true);
    plt::legend();
    plt::save("task4_orbit.png");
    plt::close();

    plt::figure();
    plt::named_plot("velocity", component(numeric, 2), component(numeric, 3));
    plt::title("Velocity phase portrait");
    plt::xlabel("x'");
    plt::ylabel("y'");
    plt::grid(true);
    plt::legend();
    plt::save("task4_velocity.png");
    plt::close();

    std::cout << "Saved task4_orbit.png, task4_velocity.png and task4_arenstorf_orbit.csv\n";
}

void print_usage(const char* program_name)
{
    std::cout
        << "Usage: " << program_name << " [test|errors|orbit|all]\n"
        << "  test   - task 2: solve test problem with exact solution\n"
        << "  errors - task 3: build e(h) and e/h^5 graphs\n"
        << "  orbit  - task 4: build Arenstorf orbit and velocity graph\n"
        << "  all    - run all tasks\n";
}

} // namespace coursework

int main(int argc, char* argv[])
{
    try
    {
        std::string mode = argc > 1 ? argv[1] : "all";

        if (mode == "test")
        {
            coursework::run_task2();
        }
        else if (mode == "errors")
        {
            coursework::run_task3();
        }
        else if (mode == "orbit")
        {
            coursework::run_task4();
        }
        else if (mode == "all")
        {
            coursework::run_task2();
            coursework::run_task3();
            coursework::run_task4();
        }
        else
        {
            coursework::print_usage(argv[0]);
            return 1;
        }

        std::cout << "Program finished" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

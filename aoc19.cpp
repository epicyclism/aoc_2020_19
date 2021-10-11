#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <tuple>
#include <regex>

#include "../../compile-time-regular-expressions/single-header/ctre.hpp"

constexpr auto rule_rx = ctll::fixed_string{ R"_((\d+): (.*))_" };

struct rule
{
    std::string id_;
    bool applied_ ;
    std::string val_;
};

void apply(std::vector<rule>& rv, rule& r)
{
    auto make_sub = [](std::string const& val) -> std::string
    {
        if (val.find('|') == std::string::npos)
            return val;
        else
            return std::string("(") + val + ")";
    };
    auto sub = make_sub(r.val_);
    for (auto& ra : rv)
    {
        if (!ra.applied_)
        {
            std::string::size_type pos = 0;
            while (( pos = ra.val_.find(r.id_, pos)) != std::string::npos)
            {
                if (ra.val_[pos - 1] == ' ' && (ra.val_[pos + r.id_.size()] == ' ' || ra.val_[pos + r.id_.size()] == '+'))
                    ra.val_.replace(pos, r.id_.size(), sub);
                pos += 2;
            }
        }
    }
    r.applied_ = true;
}

std::string decay_rules(std::vector<rule> rv)
{
    while (std::any_of(rv.begin(), rv.end(), [](auto& r) { return !r.applied_; }))
    {
        auto itr = std::find_if(rv.begin(), rv.end(), [](auto& r) { return !r.applied_ && (r.val_.find_first_of("0123456789") == std::string::npos); });
        if (itr == rv.end())
        {
            std::cout << "TILT\n";
            return "";
        };
        apply(rv, *itr);
    }
    std::string out(rv[0].val_);
    out.erase(std::remove_if(out.begin(), out.end(), [](auto c) { return c == ' '; }), out.end());
    return out;
}

std::tuple <std::string, std::string, std::vector<std::string>> get_input()
{
    std::vector<rule> rv;
    std::vector<std::string> mv;
    std::string ln;
    bool rules{ true };
    while (std::getline(std::cin, ln))
    {
        if (ln.empty())
            rules = false;
        else
            if (rules)
            {
                auto [m, id, r] = ctre::match<rule_rx>(ln);
                rv.emplace_back(std::move(id.to_string()), false, std::move(r.to_string()));
            }
            else
                mv.emplace_back(std::move(ln));
    }
    for (auto& r : rv)
    {
        if (r.val_[0] == '\"')
        {
            r.val_.erase(2, 1);
            r.val_.erase(0, 1); // assumption
        }
        else
        {
            r.val_.insert(0, 1, ' ');
            r.val_ += ' ';
        }
    }
    std::sort(rv.begin(), rv.end(), [](auto& l, auto& r) { return l.id_ < r.id_; });
    auto out1 = decay_rules(rv);
    // replace for pt2
    auto r8 = std::find_if(rv.begin(), rv.end(), [](auto& r) { return r.id_ == "8"; });
    (*r8).val_ = " 42+ ";
    auto r11 = std::find_if(rv.begin(), rv.end(), [](auto& r) { return r.id_ == "11"; });
    (*r11).val_ = " 42 31 | 42 42 31 31 | 42 42 42 31 31 31 | 42 42 42 42 31 31 31 31 | 42 42 42 42 42 31 31 31 31 31 ";
    auto out2 = decay_rules(rv);
    return { out1, out2, mv };
}

auto pt12(std::string rx, std::vector<std::string> const& v)
{
    std::regex rgx(rx);
    return std::count_if(v.begin(), v.end(), [&](auto const& s) { return std::regex_match(s, rgx); });
}

int main()
{
    auto[re1, re2, msgs] = get_input();
    auto p1 = pt12(re1, msgs);
    auto p2 = pt12(re2, msgs);
    std::cout << "part1   " << p1 << '\n';
    std::cout << "part2   " << p2 << '\n';
}

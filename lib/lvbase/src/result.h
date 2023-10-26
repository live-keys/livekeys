#ifndef LVRESULT_H
#define LVRESULT_H

#include <optional>
#include <utility>

#include "live/utf8.h"
#include "live/exception.h"
#include "live/lvbaseglobal.h"

namespace lv{

using ExceptionReport = std::vector<lv::Exception>;
using MessageReport   = std::vector<std::string>;

template<typename T, typename ReportT = ExceptionReport>
class ResultWithReport{

public:
    using Type = T;
    using ReportType = ReportT;

public:
    ResultWithReport(const T& result)
        : m_result(std::make_pair(result, std::optional<ReportT>()))
    {}
    ResultWithReport(const T& result, const ReportT& report)
        : m_result(std::make_pair(result, report))
    {}

    const T& value() const{ return m_result.first; }
    T& value(){ return m_result.first; }

    T& operator*(){ return m_result.first; }

    bool hasReport() const{ return m_result.second.has_value(); }
    const ReportT& report() const{ return m_result.second.value(); }
    ReportT& report(){ return m_result.second.value(); }
    void assignReport(const ReportT& r){ m_result.second = r; }

private:
    std::pair<T, std::optional<ReportT> > m_result;
};

} // namespace

#endif

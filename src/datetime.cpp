#include "datetime.h"
#include "live/visuallog.h"

#include "date/date.h"
#include <chrono>
#include <ctime>
#include <mutex>
#include "time.h"

namespace lv{

// class DateTimePrivate
// ----------------------------------------------------------------------------


class DateTimePrivate{
public:
    DateTimePrivate(): isLocal(false){}

    date::hh_mm_ss<std::chrono::milliseconds> totalMsInDay() const;
    std::string padNumber(int number, int len, char padChar = ' ');
    std::string dayOfWeekLongName();
    std::string dayOfWeekShortName();
    std::string monthLongName();
    std::string monthShortName();

    static std::mutex& localTimeMutex(){ static std::mutex m; return m; }

    bool isLocal;
    std::chrono::system_clock::time_point tp;
};


date::hh_mm_ss<std::chrono::milliseconds> DateTimePrivate::totalMsInDay() const{
    auto startOfDay = date::floor<date::days>(tp);
    std::chrono::milliseconds tod = std::chrono::duration_cast<std::chrono::milliseconds>(tp - startOfDay);
    return date::hh_mm_ss<std::chrono::milliseconds>{tod};
}

std::string DateTimePrivate::padNumber(int number, int len, char padChar){
    std::string res = std::to_string(number);
    if ( res.size() < static_cast<size_t>(len) ){
        return std::string(len - res.size(), padChar) + res;
    }
    return res;
}

std::string DateTimePrivate::dayOfWeekLongName(){
    std::stringstream ss;
    char fmt[] = {'%', 'A', 0};
    date::weekday wd = date::year_month_weekday{date::floor<date::days>(tp)}.weekday();
    date::to_stream(ss, fmt, wd);
    return ss.str();
}

std::string DateTimePrivate::dayOfWeekShortName(){
    std::stringstream ss;
    char fmt[] = {'%', 'a', 0};
    date::weekday wd = date::year_month_weekday{date::floor<date::days>(tp)}.weekday();
    date::to_stream(ss, fmt, wd);
    return ss.str();
}

std::string DateTimePrivate::monthLongName(){
    std::stringstream ss;
    char fmt[] = {'%', 'B', 0};
    date::month mn = date::year_month_day{date::floor<date::days>(tp)}.month();
    date::to_stream(ss, fmt, mn);
    return ss.str();
}

std::string DateTimePrivate::monthShortName(){
    std::stringstream ss;
    char fmt[] = {'%', 'b', 0};
    date::month mn = date::year_month_day{date::floor<date::days>(tp)}.month();
    date::to_stream(ss, fmt, mn);
    return ss.str();
}


// class DateTime
// ----------------------------------------------------------------------------


DateTime::DateTime()
    : m_d(new DateTimePrivate)
{
    m_d->tp = std::chrono::system_clock::now();
}

DateTime::DateTime(size_t msecs)
    : m_d(new DateTimePrivate)
{
    m_d->tp = std::chrono::system_clock::time_point{std::chrono::milliseconds(msecs)};
}

DateTime::DateTime(const DateTime &other)
    : m_d(new DateTimePrivate)
{
    m_d->tp = other.m_d->tp;
    m_d->isLocal = other.m_d->isLocal;
}

DateTime::DateTime(int y, int m, int d, int hh, int mm, int ss, int ms)
    : m_d(new DateTimePrivate)
{
    auto ymd = (date::year{y}/m/d);
    m_d->tp =
        date::sys_days(ymd) +
        std::chrono::hours(hh) +
        std::chrono::minutes(mm) +
        std::chrono::seconds(ss) +
        std::chrono::milliseconds(ms);
}

DateTime::~DateTime(){
    delete m_d;
}

int DateTime::day() const{
    return static_cast<int>(static_cast<unsigned>(date::year_month_day{date::floor<date::days>(m_d->tp)}.day()));
}

int DateTime::month() const{
    return static_cast<int>(static_cast<unsigned>(date::year_month_day{date::floor<date::days>(m_d->tp)}.month()));
}

int DateTime::year() const{
    return static_cast<int>(date::year_month_day{date::floor<date::days>(m_d->tp)}.year());
}

int DateTime::dayOfWeek() const{
    return static_cast<int>(date::year_month_weekday{date::floor<date::days>(m_d->tp)}.weekday().c_encoding());
}

int DateTime::dayOfYear() const{
    std::chrono::system_clock::time_point startOfYear = date::sys_days(date::year{year()} / 1 / 1);
    auto resultDiff = date::floor<date::days>(m_d->tp - startOfYear);
    return resultDiff.count() + 1;
}

int DateTime::hour() const{
    return m_d->totalMsInDay().hours().count();
}

int DateTime::hourAMPM() const{
    int h = hour();
    if (h < 1)
        return 12;
    else if (h > 12)
        return h - 12;
    else
        return h;
}

bool DateTime::isAM() const{
    return hour() < 12;
}

bool DateTime::isPM() const{
    return hour() >= 12;
}

int DateTime::minute() const{
    return m_d->totalMsInDay().minutes().count();
}

int DateTime::second() const{
    return m_d->totalMsInDay().seconds().count();
}

int DateTime::msecond() const{
    return m_d->totalMsInDay().subseconds().count();
}

bool DateTime::isLocal() const{
    return m_d->isLocal;
}

/**
 * \brief Returns a DateTime in local format. This function is not currently
 * thread safe.
 */
DateTime DateTime::toLocal() const{
    if ( m_d->isLocal )
        return *this;;
    std::time_t utcTime = std::chrono::system_clock::to_time_t(m_d->tp);

    DateTimePrivate::localTimeMutex().lock();
    std::tm* t = std::localtime(&utcTime);
    DateTime dt(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, msecond());
    DateTimePrivate::localTimeMutex().unlock();

    dt.m_d->isLocal = true;
    return dt;
}

DateTime &DateTime::operator =(const DateTime &other){
    m_d->isLocal = other.m_d->isLocal;
    m_d->tp = other.m_d->tp;
    return *this;
}

bool DateTime::operator ==(const DateTime &dateTime) const{
    return m_d->tp == dateTime.m_d->tp;
}

bool DateTime::operator !=(const DateTime &dateTime) const{
    return m_d->tp != dateTime.m_d->tp;
}

bool DateTime::operator <(const DateTime &dateTime) const{
    return m_d->tp < dateTime.m_d->tp;
}

bool DateTime::operator <=(const DateTime &dateTime) const{
    return m_d->tp <= dateTime.m_d->tp;
}

bool DateTime::operator >(const DateTime &dateTime) const{
    return m_d->tp > dateTime.m_d->tp;
}

bool DateTime::operator >=(const DateTime &dateTime) const{
    return m_d->tp >= dateTime.m_d->tp;
}

std::string DateTime::toString() const{
    return format("%Y-%m-%d %H:%M:%S.%i");
}

std::string DateTime::format(const std::string &pattern) const{
    std::stringstream base;

    std::string::const_iterator it = pattern.begin();
    while ( it != pattern.end() ){
         if ( *it == '%' ){
            ++it;
            if ( it != pattern.end() ){
                std::string r = formatSymbol(*it);
                if ( r.empty() )
                    base << *it;
                else
                    base << r;
            } else {
                base << '%';
                break;
            }
        } else {
            base << *it;
        }
        ++it;
    }
    return base.str();
}

std::string DateTime::formatSymbol(char symbol) const{
    switch(symbol){
    case 'w' : return m_d->dayOfWeekShortName();
    case 'W' : return m_d->dayOfWeekLongName();
    case 'b': return m_d->monthShortName();
    case 'B': return m_d->monthLongName();
    case 'd': return m_d->padNumber(day(), 2, '0');
    case 'e': return m_d->padNumber(day(), 0);
    case 'f': return m_d->padNumber(day(), 2);
    case 'm': return m_d->padNumber(month(), 2, '0');
    case 'n': return m_d->padNumber(month(), 0);
    case 'o': return m_d->padNumber(month(), 2);
    case 'y': return m_d->padNumber(year() % 100, 2, '0');
    case 'Y': return m_d->padNumber(year(), 4, '0');
    case 'H': return m_d->padNumber(hour(), 2, '0');
    case 'I': return m_d->padNumber(hourAMPM() < 1 ? 12 : hourAMPM(), 2, '0');
    case 'a': return isAM() ? "am" : "pm";
    case 'A': return isAM() ? "AM" : "PM";
    case 'M': return m_d->padNumber(minute(), 2, '0');
    case 'S': return m_d->padNumber(second(), 2, '0');
    case 's': return m_d->padNumber(second(), 2, '0') + "." + m_d->padNumber(msecond(), 3, '0');
    case 'i': return m_d->padNumber(msecond(), 3, '0');
    case 'c': return m_d->padNumber(msecond() / 100, 0);
    }
    return "";
}

DateTime DateTime::addMSeconds(int count) const{
    std::chrono::milliseconds msecs(count);
    std::chrono::system_clock::time_point newTp = m_d->tp + msecs;

    DateTime dt(0);
    dt.m_d->tp = newTp;
    return dt;
}

DateTime DateTime::addDays(int count) const{
    date::days days(count);
    std::chrono::system_clock::time_point newTp = m_d->tp + days;

    DateTime dt(0);
    dt.m_d->tp = newTp;
    return dt;
}

DateTime DateTime::create(int year, int m, int d, int hh, int mm, int ss, int ms){
    auto ymd = (date::year{year}/m/d);
    if ( !ymd.ok() ){
        THROW_EXCEPTION(lv::Exception, Utf8("Invalid date given: %-%-%").format(year, m, d), lv::Exception::toCode("~Date"));
    }
    if ( hh > 23 || mm > 59 || ss > 59 || ms > 999 ){
        THROW_EXCEPTION(lv::Exception, Utf8("Invalid time given: %:%:%.%").format(hh, mm, ss, ms), lv::Exception::toCode("~Time"));
    }
    return DateTime(year, m, d, hh, mm, ss, ms);
}

DateTime DateTime::createFromMs(size_t timeSinceEpoch){
    return DateTime(timeSinceEpoch);
}

VisualLog &operator <<(VisualLog &vl, const DateTime &value){
    return vl << value.toString().c_str();
}

}// namespace

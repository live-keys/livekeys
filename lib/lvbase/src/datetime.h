#ifndef LVDATETIME_H
#define LVDATETIME_H

#include "live/lvbaseglobal.h"
#include "live/utf8.h"

namespace lv{

class DateTimePrivate;
class LV_BASE_EXPORT DateTime{

public:
    DateTime();
    DateTime(int year, int m, int d, int hh = 0, int mm = 0, int ss = 0, int ms = 0);
    DateTime(const DateTime& other);

    ~DateTime();

    int day() const;
    int month() const;
    int year() const;
    int dayOfWeek() const;
    int dayOfYear() const;

    int hour() const;
    int hourAMPM() const;
    bool isAM() const;
    bool isPM() const;
    int minute() const;
    int second() const;
    int msecond() const;

    bool isLocal() const;
    DateTime toLocal() const;

    DateTime& operator = (const DateTime& other);

    bool operator == (const DateTime& dateTime) const;
    bool operator != (const DateTime& dateTime) const;
    bool operator <  (const DateTime& dateTime) const;
    bool operator <= (const DateTime& dateTime) const;
    bool operator >  (const DateTime& dateTime) const;
    bool operator >= (const DateTime& dateTime) const;

    std::string toString() const;
    std::string format(const std::string& fmt) const;
    std::string formatSymbol(char symbol) const;

    DateTime addMSeconds(int count) const;
    DateTime addDays(int count) const;

    static DateTime create(int year, int m = 1, int d = 1, int hh = 0, int mm = 0, int ss = 0, int ms = 0);
    static DateTime createFromMs(size_t timeSinceEpoch);

private:
    DateTime(size_t timeSinceEpoch);

    DateTimePrivate* m_d;
};

LV_BASE_EXPORT VisualLog &operator <<(VisualLog &vl, const DateTime& value);

}// namespace

#endif // DATETIME_H

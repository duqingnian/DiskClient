#ifndef META_H
#define META_H
#include <QString>

struct EMP{
    QString name="";
    QString dep_id="";   //部门ID
    QString dep_name=""; //部门名称
    QString job_number="";
};

class Meta
{
public:
    Meta();
};

#endif // META_H

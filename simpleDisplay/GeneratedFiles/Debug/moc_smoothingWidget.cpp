/****************************************************************************
** Meta object code from reading C++ file 'smoothingWidget.h'
**
** Created: Thu 3. Jan 15:41:28 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../smoothingWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'smoothingWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_smoothingWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      34,   16,   16,   16, 0x0a,
      57,   16,   16,   16, 0x0a,
      71,   16,   16,   16, 0x0a,
      96,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_smoothingWidget[] = {
    "smoothingWidget\0\0updateTimeStep()\0"
    "startDirectSmoothing()\0doSmoothing()\0"
    "startImplicitSmoothing()\0doImplicitSmoothing()\0"
};

void smoothingWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        smoothingWidget *_t = static_cast<smoothingWidget *>(_o);
        switch (_id) {
        case 0: _t->updateTimeStep(); break;
        case 1: _t->startDirectSmoothing(); break;
        case 2: _t->doSmoothing(); break;
        case 3: _t->startImplicitSmoothing(); break;
        case 4: _t->doImplicitSmoothing(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData smoothingWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject smoothingWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_smoothingWidget,
      qt_meta_data_smoothingWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &smoothingWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *smoothingWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *smoothingWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_smoothingWidget))
        return static_cast<void*>(const_cast< smoothingWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int smoothingWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

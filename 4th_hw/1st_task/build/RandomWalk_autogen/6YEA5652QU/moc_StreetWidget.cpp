/****************************************************************************
** Meta object code from reading C++ file 'StreetWidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/StreetWidget.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StreetWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_RandomWalk__StreetWidget_t {
    uint offsetsAndSizes[26];
    char stringdata0[25];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[7];
    char stringdata5[11];
    char stringdata6[15];
    char stringdata7[11];
    char stringdata8[11];
    char stringdata9[2];
    char stringdata10[5];
    char stringdata11[7];
    char stringdata12[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RandomWalk__StreetWidget_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RandomWalk__StreetWidget_t qt_meta_stringdata_RandomWalk__StreetWidget = {
    {
        QT_MOC_LITERAL(0, 24),  // "RandomWalk::StreetWidget"
        QT_MOC_LITERAL(25, 17),  // "animationFinished"
        QT_MOC_LITERAL(43, 0),  // ""
        QT_MOC_LITERAL(44, 10),  // "WalkResult"
        QT_MOC_LITERAL(55, 6),  // "result"
        QT_MOC_LITERAL(62, 10),  // "totalSteps"
        QT_MOC_LITERAL(73, 14),  // "loadTrajectory"
        QT_MOC_LITERAL(88, 10),  // "QList<int>"
        QT_MOC_LITERAL(99, 10),  // "trajectory"
        QT_MOC_LITERAL(110, 1),  // "B"
        QT_MOC_LITERAL(112, 4),  // "stop"
        QT_MOC_LITERAL(117, 6),  // "replay"
        QT_MOC_LITERAL(124, 12)   // "advanceFrame"
    },
    "RandomWalk::StreetWidget",
    "animationFinished",
    "",
    "WalkResult",
    "result",
    "totalSteps",
    "loadTrajectory",
    "QList<int>",
    "trajectory",
    "B",
    "stop",
    "replay",
    "advanceFrame"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RandomWalk__StreetWidget[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   44,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    3,   49,    2, 0x0a,    4 /* Public */,
      10,    0,   56,    2, 0x0a,    8 /* Public */,
      11,    0,   57,    2, 0x0a,    9 /* Public */,
      12,    0,   58,    2, 0x08,   10 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int, 0x80000000 | 3,    8,    9,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RandomWalk::StreetWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RandomWalk__StreetWidget.offsetsAndSizes,
    qt_meta_data_RandomWalk__StreetWidget,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RandomWalk__StreetWidget_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<StreetWidget, std::true_type>,
        // method 'animationFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<WalkResult, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'loadTrajectory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QList<int>, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<WalkResult, std::false_type>,
        // method 'stop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'replay'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'advanceFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RandomWalk::StreetWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StreetWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->animationFinished((*reinterpret_cast< std::add_pointer_t<WalkResult>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->loadTrajectory((*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<WalkResult>>(_a[3]))); break;
        case 2: _t->stop(); break;
        case 3: _t->replay(); break;
        case 4: _t->advanceFrame(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (StreetWidget::*)(WalkResult , int );
            if (_t _q_method = &StreetWidget::animationFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *RandomWalk::StreetWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RandomWalk::StreetWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RandomWalk__StreetWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RandomWalk::StreetWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RandomWalk::StreetWidget::animationFinished(WalkResult _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

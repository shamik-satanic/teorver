/****************************************************************************
** Meta object code from reading C++ file 'SimulationWorker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/SimulationWorker.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SimulationWorker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PointMotion__SimulationWorker_t {
    uint offsetsAndSizes[30];
    char stringdata0[30];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[23];
    char stringdata4[7];
    char stringdata5[10];
    char stringdata6[7];
    char stringdata7[29];
    char stringdata8[4];
    char stringdata9[14];
    char stringdata10[4];
    char stringdata11[8];
    char stringdata12[23];
    char stringdata13[7];
    char stringdata14[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PointMotion__SimulationWorker_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PointMotion__SimulationWorker_t qt_meta_stringdata_PointMotion__SimulationWorker = {
    {
        QT_MOC_LITERAL(0, 29),  // "PointMotion::SimulationWorker"
        QT_MOC_LITERAL(30, 11),  // "singleReady"
        QT_MOC_LITERAL(42, 0),  // ""
        QT_MOC_LITERAL(43, 22),  // "PointMotion::SimResult"
        QT_MOC_LITERAL(66, 6),  // "result"
        QT_MOC_LITERAL(73, 9),  // "fullReady"
        QT_MOC_LITERAL(83, 6),  // "sample"
        QT_MOC_LITERAL(90, 28),  // "PointMotion::EmpiricalResult"
        QT_MOC_LITERAL(119, 3),  // "emp"
        QT_MOC_LITERAL(123, 13),  // "errorOccurred"
        QT_MOC_LITERAL(137, 3),  // "msg"
        QT_MOC_LITERAL(141, 7),  // "runFull"
        QT_MOC_LITERAL(149, 22),  // "PointMotion::SimParams"
        QT_MOC_LITERAL(172, 6),  // "params"
        QT_MOC_LITERAL(179, 9)   // "runSingle"
    },
    "PointMotion::SimulationWorker",
    "singleReady",
    "",
    "PointMotion::SimResult",
    "result",
    "fullReady",
    "sample",
    "PointMotion::EmpiricalResult",
    "emp",
    "errorOccurred",
    "msg",
    "runFull",
    "PointMotion::SimParams",
    "params",
    "runSingle"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PointMotion__SimulationWorker[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       5,    2,   47,    2, 0x06,    3 /* Public */,
       9,    1,   52,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    1,   55,    2, 0x0a,    8 /* Public */,
      14,    1,   58,    2, 0x0a,   10 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 7,    6,    8,
    QMetaType::Void, QMetaType::QString,   10,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject PointMotion::SimulationWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PointMotion__SimulationWorker.offsetsAndSizes,
    qt_meta_data_PointMotion__SimulationWorker,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PointMotion__SimulationWorker_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SimulationWorker, std::true_type>,
        // method 'singleReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<PointMotion::SimResult, std::false_type>,
        // method 'fullReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<PointMotion::SimResult, std::false_type>,
        QtPrivate::TypeAndForceComplete<PointMotion::EmpiricalResult, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'runFull'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<PointMotion::SimParams, std::false_type>,
        // method 'runSingle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<PointMotion::SimParams, std::false_type>
    >,
    nullptr
} };

void PointMotion::SimulationWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SimulationWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->singleReady((*reinterpret_cast< std::add_pointer_t<PointMotion::SimResult>>(_a[1]))); break;
        case 1: _t->fullReady((*reinterpret_cast< std::add_pointer_t<PointMotion::SimResult>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<PointMotion::EmpiricalResult>>(_a[2]))); break;
        case 2: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->runFull((*reinterpret_cast< std::add_pointer_t<PointMotion::SimParams>>(_a[1]))); break;
        case 4: _t->runSingle((*reinterpret_cast< std::add_pointer_t<PointMotion::SimParams>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< PointMotion::SimResult >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< PointMotion::EmpiricalResult >(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< PointMotion::SimResult >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< PointMotion::SimParams >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< PointMotion::SimParams >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SimulationWorker::*)(PointMotion::SimResult );
            if (_t _q_method = &SimulationWorker::singleReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SimulationWorker::*)(PointMotion::SimResult , PointMotion::EmpiricalResult );
            if (_t _q_method = &SimulationWorker::fullReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SimulationWorker::*)(QString );
            if (_t _q_method = &SimulationWorker::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *PointMotion::SimulationWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PointMotion::SimulationWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PointMotion__SimulationWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PointMotion::SimulationWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void PointMotion::SimulationWorker::singleReady(PointMotion::SimResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PointMotion::SimulationWorker::fullReady(PointMotion::SimResult _t1, PointMotion::EmpiricalResult _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PointMotion::SimulationWorker::errorOccurred(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

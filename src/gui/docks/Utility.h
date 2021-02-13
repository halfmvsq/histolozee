#ifndef GUI_DOCK_UTILITY_H
#define GUI_DOCK_UTILITY_H

#include <glm/mat4x4.hpp>

#include <QLayout>
#include <QObject>
#include <QWidget>

#include <type_traits>

class ctkMatrixWidget;


namespace gui
{

void setZeroContentsMargins( QWidget* widget, bool zeroLeft, bool zeroTop, bool zeroRight, bool zeroBottom );

void setZeroContentsMargins( QLayout* layout, bool zeroLeft, bool zeroTop, bool zeroRight, bool zeroBottom );

void expandContentsMargins( QLayout* layout, int addLeft, int addTop, int addRight, int addBottom );

void setMatrixWidgetValues( ctkMatrixWidget* widget, const glm::dmat4& m );


/**
 * @brief This class blocks signals from a Qt object before returning a pointer to it.
 *
 * @todo Use the Qt version provided in QSignalBlocker.h
 *
 * @example QSignalBlocker( mySlider )->setValue( 100 );
 */
template< class T >
class QSignalBlocker
{
    static_assert( std::is_base_of<QObject, T>::value, "T must derive from QObject" );

public:

    explicit QSignalBlocker( T* obj )
        : m_obj( obj ) {}

    ~QSignalBlocker()
    {
        if ( m_obj )
        {
            m_obj->blockSignals( false );
        }
    }

    T* operator->()
    {
        if ( m_obj )
        {
            m_obj->blockSignals( true );
        }
        return m_obj;
    }

private:

    T* const m_obj;
};


template< class T >
QSignalBlocker<T> SilentCall( T* obj )
{
    return QSignalBlocker<T>( obj );
}


template< class T >
class QSignalBlocker2
{
    static_assert( std::is_base_of<QObject, T>::value, "T must derive from QObject" );

public:

    explicit QSignalBlocker2( T* obj ) : m_obj( obj )
    {
        if ( m_obj ) m_obj->blockSignals( true );
    }

    ~QSignalBlocker2()
    {
        if ( m_obj ) m_obj->blockSignals( false );
    }

private:

    T* const m_obj;
};

} // namespace gui

#endif // GUI_DOCK_UTILITY_H

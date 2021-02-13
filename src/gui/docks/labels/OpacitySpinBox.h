#ifndef GUI_OPACITY_SPINBOX_H
#define GUI_OPACITY_SPINBOX_H

#include <QSpinBox>

class QWidget;


/**
 * @brief A QSpinBox specialized to represent opacity values in range [0, 100]
 * with single step increments of 1.
 */
class OpacitySpinBox : public QSpinBox
{
    Q_OBJECT

public:

    OpacitySpinBox( QWidget* parent = nullptr );
    ~OpacitySpinBox() override = default;
};

#endif // GUI_OPACITY_SPINBOX_H

#ifndef GUI_OPACITY_DOUBLE_SPINBOX_H
#define GUI_OPACITY_DOUBLE_SPINBOX_H

#include <QDoubleSpinBox>

class QWidget;


/**
 * @brief A QDoubleSpinBox specialized to represent opacity values in [0.0, 1.0]
 * with single step increments of 0.01.
 */
class OpacityDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:

    OpacityDoubleSpinBox( QWidget* parent = nullptr );
    ~OpacityDoubleSpinBox() override = default;
};

#endif // GUI_OPACITY_DOUBLE_SPINBOX_H

#pragma once

#include <DCViz/include/DCViz.h>


namespace ignis
{

template<typename pT>
class LauchDCViz : public Event<pT>
{

public:

    LauchDCViz(const std::string path,
               const double delay = 0.0,
               const bool dynamic = true,
               const int sx = 16,
               const int sy = 14) :
        Event<pT>("DCViz"),
        m_delay(delay),
        m_viz(path),
        m_dynamic(dynamic),
        m_sx(sx),
        m_sy(sy)
    {

    }

    void initialize()
    {
        m_viz.launch(m_dynamic, m_delay, m_sx, m_sy);
    }

    void execute() {}

private:

    const double m_delay;

    DCViz m_viz;

    const bool m_dynamic;

    const int m_sx;

    const int m_sy;

};

}

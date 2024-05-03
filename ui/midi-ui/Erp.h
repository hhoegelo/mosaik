#include <gtkmm.h>
#include <optional>

namespace Ui::Midi
{
  class Erp : public Gtk::DrawingArea
  {
   public:
    Erp();

    sigc::connection connect(const std::function<void(int)>& cb);
    sigc::connection down(const std::function<void()>& cb);
    sigc::connection up(const std::function<void()>& cb);

   protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_button_press_event(GdkEventButton* event) override;
    bool on_button_release_event(GdkEventButton* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;

   private:
    using Point = std::tuple<double, double>;
    std::optional<Point> m_lastMousePos;
    double m_degree = 0;
    sigc::signal<void, double> m_turn;
    sigc::signal<void> m_down;
    sigc::signal<void> m_up;
  };
}

#include "Window.h"
#include <core/api/Interface.h>

namespace Ui
{
  namespace Touch
  {
    class StepButton : public Gtk::Button
    {
     public:
      StepButton(Core::Api::Interface& core, int channel, int step)
          : Gtk::Button()
      {
        signal_clicked().connect(
            [&core, channel, step, this]
            {
              auto pattern = m_currentPattern;
              pattern[step] = !pattern[step];
              core.setParameter(channel, Core::Api::ParameterId::Pattern, pattern);
            });

        m_connection = core.connect(channel, Core::Api::ParameterId::Pattern,
                                    [this, step](const Core::Api::ParameterValue& p)
                                    {
                                      m_currentPattern = std::get<Core::Api::Pattern>(p);
                                      this->set_label(m_currentPattern[step] ? "x" : " ");
                                    });
      }

     private:
      Tools::Signals::Connection m_connection;
      Core::Api::Pattern m_currentPattern;
    };

    class Channel : public Gtk::Box
    {
     public:
      Channel(Gtk::Window& wnd, Core::Api::Interface& core, int channel)
          : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 2)
      {
        auto load = Gtk::manage(new Gtk::Button("Load"));
        pack_start(*load, Gtk::PACK_SHRINK);

        for(auto s = 0; s < NUM_STEPS; s++)
        {
          auto step = Gtk::manage(new StepButton(core, channel, s));
          pack_start(*step, Gtk::PACK_SHRINK);
        }

        load->signal_clicked().connect(
            [this, &wnd, &core, channel]
            {
              Gtk::FileChooserDialog dialog(wnd, "Select audio file...");

              dialog.set_transient_for(wnd);
              dialog.set_modal(true);

              dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
              dialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);

              if(dialog.run() == Gtk::RESPONSE_ACCEPT)
              {
                core.setParameter(Core::Api::ChannelId { channel }, Core::Api::ParameterId::SampleFile,
                                  dialog.get_filename());
              }
            });
      }
    };

    Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
    {
      set_title("Mosaik");
      set_border_width(10);

      auto hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2));

      for(auto c = 0; c < NUM_CHANNELS; c++)
        hbox->pack_start(*Gtk::manage(new Channel(*this, core, c)), Gtk::PACK_SHRINK);

      add(*hbox);
      show_all();
    }
  }
}
#include "Window.h"
#include <core/api/Interface.h>
#include <iostream>

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
        auto connection = signal_clicked().connect(
            [&core, channel, step, this]
            {
              auto pattern = m_currentPattern;
              pattern[step] = !pattern[step];
              core.setParameter(channel, Core::Api::ParameterId::Pattern, pattern);
            });

        m_connection = core.connect(channel, Core::Api::ParameterId::Pattern,
                                    [this, step, connection](const Core::Api::ParameterValue& p) mutable
                                    {
                                      connection.block();
                                      m_currentPattern = std::get<Core::Api::Pattern>(p);
                                      this->set_label(m_currentPattern[step] ? "x" : " ");
                                      connection.unblock();
                                    });
      }

     private:
      Tools::Signals::Connection m_connection;
      Core::Api::Pattern m_currentPattern;
    };

    class Volume : public Gtk::ScaleButton
    {
     public:
      Volume(Core::Api::Interface& core)
          : Gtk::ScaleButton(Gtk::IconSize(Gtk::ICON_SIZE_BUTTON), 0.0, 1.0, 0.01, { "stock_volume" })
      {
        auto connection = signal_value_changed().connect(
            [&core, this](double v)
            { core.setParameter({}, Core::Api::ParameterId::GlobalVolume, static_cast<float>(v)); });

        m_connection
            = core.connect({}, Core::Api::ParameterId::GlobalVolume,
                           [this, connection = std::move(connection)](const Core::Api::ParameterValue& p) mutable
                           {
                             connection.block();
                             set_value(get<float>(p));
                             connection.unblock();
                           });
      }

     private:
      Tools::Signals::Connection m_connection;
    };

    class Tempo : public Gtk::ScaleButton
    {
     public:
      Tempo(Core::Api::Interface& core)
          : Gtk::ScaleButton(Gtk::IconSize(Gtk::ICON_SIZE_BUTTON), 20, 240, 0.5, { "speedometer-symbolic" })
      {
        auto connection = signal_value_changed().connect(
            [&core, this](double v)
            { core.setParameter({}, Core::Api::ParameterId::GlobalTempo, static_cast<float>(v)); });

        m_connection = core.connect({}, Core::Api::ParameterId::GlobalTempo,
                                    [this, connection](const Core::Api::ParameterValue& p) mutable
                                    {
                                      connection.block();
                                      set_value(get<float>(p));
                                      connection.unblock();
                                    });
      }

     private:
      Tools::Signals::Connection m_connection;
    };

    class Gain : public Gtk::ScaleButton
    {
     public:
      Gain(Core::Api::Interface& core, int channel)
          : Gtk::ScaleButton(Gtk::IconSize(Gtk::ICON_SIZE_BUTTON), 0.0, 1.0, 0.01, { "stock_volume" })
      {
        auto connection = signal_value_changed().connect(
            [&core, channel, this](double v)
            { core.setParameter(channel, Core::Api::ParameterId::Gain, static_cast<float>(v)); });

        m_connection = core.connect(channel, Core::Api::ParameterId::Gain,
                                    [this, connection](const Core::Api::ParameterValue& p) mutable
                                    {
                                      connection.block();
                                      set_value(get<float>(p));
                                      connection.unblock();
                                    });
      }

     private:
      Tools::Signals::Connection m_connection;
    };

    class Balance : public Gtk::ScaleButton
    {
     public:
      Balance(Core::Api::Interface& core, int channel)
          : Gtk::ScaleButton(Gtk::IconSize(Gtk::ICON_SIZE_BUTTON), -1.0, 1.0, 0.02, { "pan-start-symbolic" })
      {
        auto connection = signal_value_changed().connect(
            [&core, channel, this](double v)
            { core.setParameter(channel, Core::Api::ParameterId::Balance, static_cast<float>(v)); });

        m_connection = core.connect(channel, Core::Api::ParameterId::Balance,
                                    [this, connection](const Core::Api::ParameterValue& p) mutable
                                    {
                                      connection.block();
                                      set_value(get<float>(p));
                                      connection.unblock();
                                    });
      }

     private:
      Tools::Signals::Connection m_connection;
    };

    class Mute : public Gtk::CheckButton
    {
     public:
      Mute(Core::Api::Interface& core, int channel)
          : Gtk::CheckButton("Mute")
      {
        auto connection = signal_toggled().connect(
            [&core, channel, this] { core.setParameter(channel, Core::Api::ParameterId::Mute, !m_state); });

        m_connection = core.connect(channel, Core::Api::ParameterId::Mute,
                                    [this, connection](const Core::Api::ParameterValue& p) mutable
                                    {
                                      m_state = get<bool>(p);
                                      connection.block();
                                      set_active(m_state);
                                      connection.unblock();
                                    });
      }

     private:
      Tools::Signals::Connection m_connection;
      bool m_state = false;
    };

    class Reverse : public Gtk::CheckButton
    {
     public:
      Reverse(Core::Api::Interface& core, int channel)
          : Gtk::CheckButton("Reverse")
      {
        auto connection = signal_toggled().connect(
            [&core, channel, this] { core.setParameter(channel, Core::Api::ParameterId::Reverse, !m_state); });

        m_connection = core.connect(channel, Core::Api::ParameterId::Reverse,
                                    [this, connection](const Core::Api::ParameterValue& p) mutable
                                    {
                                      connection.block();
                                      m_state = get<bool>(p);
                                      this->set_active(m_state);
                                      connection.unblock();
                                    });
      }

     private:
      Tools::Signals::Connection m_connection;
      bool m_state = false;
    };

    class Channel : public Gtk::Box
    {
     public:
      Channel(Gtk::Window& wnd, Core::Api::Interface& core, int channel)
          : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 2)
      {
        auto load = Gtk::manage(new Gtk::Button("Load"));
        pack_start(*load, Gtk::PACK_SHRINK);

        pack_start(*Gtk::manage(new Gain(core, channel)), Gtk::PACK_SHRINK);
        pack_start(*Gtk::manage(new Balance(core, channel)), Gtk::PACK_SHRINK);
        pack_start(*Gtk::manage(new Reverse(core, channel)), Gtk::PACK_SHRINK);
        pack_start(*Gtk::manage(new Mute(core, channel)), Gtk::PACK_SHRINK);

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

      auto globals = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 2));
      globals->pack_start(*Gtk::manage(new Volume(core)), Gtk::PACK_SHRINK);
      globals->pack_start(*Gtk::manage(new Tempo(core)), Gtk::PACK_SHRINK);

      auto stack = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2));
      stack->pack_start(*globals, Gtk::PACK_SHRINK);

      for(auto c = 0; c < NUM_CHANNELS; c++)
        stack->pack_start(*Gtk::manage(new Channel(*this, core, c)), Gtk::PACK_SHRINK);

      add(*stack);
      show_all();
    }
  }
}
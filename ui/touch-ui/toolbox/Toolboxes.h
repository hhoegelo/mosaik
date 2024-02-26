#pragma once

#include <gtkmm/notebook.h>
#include <optional>
#include "core/Types.h"

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Toolboxes : public Gtk::Notebook
  {
   public:
    explicit Toolboxes(Core::Api::Interface &core);

   private:
    Widget *buildScale(std::optional<uint32_t> tile, Core::ParameterId id, double min, double max);
    Widget *buildGlobals();
    Widget *buildFileBrowser();

    Core::Api::Interface &m_core;
    Glib::RefPtr<Gio::File> m_lastSelectedFolder;
  };
}
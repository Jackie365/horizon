#pragma once
#include <gtkmm.h>
#include "common/common.hpp"
#include "editor_interface.hpp"
#include "util/sort_helper.hpp"

namespace horizon {

class UnitEditor : public Gtk::Box, public PoolEditorInterface {
    friend class PinEditor;

public:
    UnitEditor(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &x, class Unit &u, class IPool &p);
    static UnitEditor *create(class Unit &u, class IPool &p);
    void select(const ItemSet &items) override;

    virtual ~UnitEditor(){};

private:
    class Unit &unit;
    Gtk::Entry *name_entry = nullptr;
    Gtk::Entry *manufacturer_entry = nullptr;
    Gtk::ListBox *pins_listbox = nullptr;
    Gtk::Button *add_button = nullptr;
    Gtk::Button *delete_button = nullptr;
    Gtk::CheckButton *cross_probing_cb = nullptr;

    Glib::RefPtr<Gtk::SizeGroup> sg_direction;
    Glib::RefPtr<Gtk::SizeGroup> sg_name;
    Glib::RefPtr<Gtk::SizeGroup> sg_names;

    void handle_add();
    void handle_delete();
    void sort();
    void handle_activate(class PinEditor *ed);

    SortHelper sort_helper;

    IPool &pool;
};
} // namespace horizon

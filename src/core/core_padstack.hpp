#pragma once
#include "core.hpp"
#include "pool/padstack.hpp"
#include "document/idocument_padstack.hpp"

namespace horizon {
class CorePadstack : public Core, public virtual IDocumentPadstack {
public:
    CorePadstack(const std::string &filename, IPool &pool);
    bool has_object_type(ObjectType ty) const override;

    class LayerProvider &get_layer_provider() override;

    Padstack &get_padstack() override;

    bool set_property(ObjectType type, const UUID &uu, ObjectProperty::ID property,
                      const class PropertyValue &value) override;
    bool get_property(ObjectType type, const UUID &uu, ObjectProperty::ID property,
                      class PropertyValue &value) override;
    bool get_property_meta(ObjectType type, const UUID &uu, ObjectProperty::ID property,
                           class PropertyMeta &meta) override;

    std::string get_display_name(ObjectType type, const UUID &uu) override;

    const Padstack &get_canvas_data();
    std::pair<Coordi, Coordi> get_bbox() override;

    const std::string &get_filename() const override;

    ObjectType get_object_type() const override
    {
        return ObjectType::PADSTACK;
    }

    const FileVersion &get_version() const override
    {
        return padstack.version;
    }

    void set_temp_mode();
    void set_filename(const std::string &filename);

private:
    std::map<UUID, Polygon> *get_polygon_map() override;
    std::map<UUID, Hole> *get_hole_map() override;

    Padstack padstack;
    std::string m_filename;

    class HistoryItem : public Core::HistoryItem {
    public:
        HistoryItem(const Padstack &s, const std::string &cm) : Core::HistoryItem(cm), padstack(s)
        {
        }
        Padstack padstack;
    };
    void rebuild_internal(bool from_undo, const std::string &comment) override;
    void history_push(const std::string &comment) override;
    void history_load(unsigned int i) override;
    void save(const std::string &suffix) override;
    void delete_autosave() override;

public:
    std::string parameter_program_code;
    ParameterSet parameter_set;
    std::set<ParameterID> parameters_required;
};
} // namespace horizon

#include "gerber_export.hpp"
#include "canvas_gerber.hpp"
#include "board/board.hpp"
#include "board/gerber_output_settings.hpp"
#include <glibmm/miscutils.h>
#include "export_util/tree_writer_archive.hpp"
#include "board/board_layers.hpp"
#include "util/util.hpp"

namespace horizon {
GerberExporter::GerberExporter(const Board &b, const GerberOutputSettings &s) : brd(b), settings(s)
{

    for (const auto &it : settings.layers) {
        if (brd.get_layers().count(it.first) && it.second.enabled)
            writers.emplace(it.first,
                            Glib::build_filename(settings.output_directory, settings.prefix + it.second.filename));
    }

    drill_writer_pth = std::make_unique<ExcellonWriter>(
            Glib::build_filename(settings.output_directory, settings.prefix + settings.drill_pth_filename));
    if (settings.drill_mode == GerberOutputSettings::DrillMode::INDIVIDUAL) {
        drill_writer_npth = std::make_unique<ExcellonWriter>(
                Glib::build_filename(settings.output_directory, settings.prefix + settings.drill_npth_filename));
    }
    else {
        drill_writer_npth = nullptr;
    }
}

void GerberExporter::generate()
{
    CanvasGerber ca(*this);
    ca.outline_width = settings.outline_width;
    ca.update(brd);

    for (auto &it : writers) {
        it.second.write_format();
        it.second.write_apertures();
        it.second.write_regions();
        it.second.write_lines();
        it.second.write_arcs();
        it.second.write_pads();
        it.second.close();
        log << "Wrote layer " << brd.get_layers().at(it.first).name << " to gerber file " << it.second.get_filename()
            << std::endl;
    }
    for (auto it : {drill_writer_npth.get(), drill_writer_pth.get()}) {
        if (it) {
            it->write_format();
            it->write_header();
            it->write_holes();
            it->close();
            log << "Wrote excellon drill file " << it->get_filename() << std::endl;
        }
    }

    if (settings.zip_output) {
        generate_zip();
    }
}

static void add_file(TreeWriter &writer, const std::string &filename)
{
    auto basename = Glib::path_get_basename(filename);
    auto file = writer.create_file(fs::u8path(basename));
    auto ifs = make_ifstream(filename, std::ios_base::in | std::ios_base::binary);
    file.stream << ifs.rdbuf();
}

void GerberExporter::generate_zip()
{
    const std::string zip_file = Glib::build_filename(settings.output_directory, settings.prefix + ".zip");
    TreeWriterArchive tree_writer(zip_file, TreeWriterArchive::Type::ZIP);

    for (auto &it : writers) {
        add_file(tree_writer, it.second.get_filename());
    }

    for (auto it : {drill_writer_npth.get(), drill_writer_pth.get()}) {
        if (it) {
            add_file(tree_writer, it->get_filename());
        }
    }
    log << "Added files to " << zip_file << std::endl;
}

std::string GerberExporter::get_log()
{
    return log.str();
}

GerberWriter *GerberExporter::get_writer_for_layer(int l)
{
    if (l == BoardLayers::OUTLINE_NOTES)
        l = BoardLayers::L_OUTLINE;

    if (writers.count(l)) {
        return &writers.at(l);
    }
    return nullptr;
}

ExcellonWriter *GerberExporter::get_drill_writer(bool pth)
{
    if (settings.drill_mode == GerberOutputSettings::DrillMode::MERGED) {
        return drill_writer_pth.get();
    }
    else {
        return pth ? drill_writer_pth.get() : drill_writer_npth.get();
    }
}
} // namespace horizon

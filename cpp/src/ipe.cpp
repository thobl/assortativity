
#include "ipe.hpp"

#include <iomanip>
#include <ios>
#include <string>

IpeFile::IpeFile(const std::string& filename, double scaling_factor,
                 double margin)
    : m_file(filename), m_scaling(scaling_factor), m_margin(margin) {
  m_file << std::fixed << std::setprecision(1);
  file_start();
  page_start();
}

IpeFile::~IpeFile() {
  page_end();
  file_end();
}

void IpeFile::new_page() {
  page_end();
  page_start();
}

void IpeFile::label(const std::string& label, double x, double y,
                    const std::string& color) {
  normalize_point(x, y);
  m_file << R"(<text layer="alpha" transformations="translations" pos=")" << x
         << " " << y << "\" stroke=\"" << color << R"(" type="label" )"
         << R"(halign="center" size="normal" valign="center">)" << label
         << "</text>\n";
}

void IpeFile::line(double x1, double y1, double x2, double y2,
                   const std::string& color,
                   const std::string& additional_settings) {
  normalize_point(x1, y1);
  normalize_point(x2, y2);
  m_file << "<path stroke = \"" << color << "\" " << additional_settings
         << ">\n";
  m_file << x1 << " " << y1 << " m\n";
  m_file << x2 << " " << y2 << " l\n";
  m_file << "</path>\n";
}

void IpeFile::box(double x1, double y1, double x2, double y2,
                  const std::string& color) {
  normalize_point(x1, y1);
  normalize_point(x2, y2);
  m_file << "<path stroke = \"" << color << "\">\n";
  m_file << x1 << " " << y1 << " m\n"
         << x1 << " " << y2 << " l\n"
         << x2 << " " << y2 << " l\n"
         << x2 << " " << y1 << " l\n"
         << "h\n";
  m_file << "</path>\n";
}

void IpeFile::point(double x, double y, const std::string& color) {
  normalize_point(x, y);
  m_file << "<use name=\"mark/disk(sx)\" pos=\"" << x << " " << y
         << R"(" size="normal" stroke=")" << color << "\"/>\n";
}

void IpeFile::disk(double x, double y, double radius,
                   const std::string& color) {
  normalize_point(x, y);
  m_file << "<path fill=\"" << color << "\">\n"
         << radius << " 0 0 " << radius << " " << x << " " << y << " e\n"
         << "</path>\n";
}

void IpeFile::start_group() { m_file << "<group>\n"; }

void IpeFile::start_group_with_clipping(double x1, double y1, double x2,
                                        double y2) {
  normalize_point(x1, y1);
  normalize_point(x2, y2);
  m_file << "<group clip=\"" << x1 << " " << y1 << " m\n"
         << x1 << " " << y2 << " l\n"
         << x2 << " " << y2 << " l\n"
         << x2 << " " << y1 << " l\n"
         << "h\n"
         << "\">\n";
}

void IpeFile::end_group() { m_file << "</group>\n"; }

void IpeFile::file_start() {
  m_file << "<?xml version=\"1.0\"?>\n"
         << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n"
         << "<ipe version=\"70206\" creator=\"Ipe 7.2.6\">\n";
  m_file << "<ipestyle name=\"dummy\">\n";
  m_file << "<symbol name=\"mark/disk(sx)\" transformations=\"translations\">\n"
         << "<path fill=\"sym-stroke\">\n"
         << "0.6 0 0 0.6 0 0 e\n"
         << "</path>\n"
         << "</symbol>\n";
  m_file << "<opacity name=\"60%\" value=\"0.6\"/>\n";
  m_file << "<textstretch name=\"normal\" value=\"1\"/>\n";
  m_file << "</ipestyle>\n";
}

void IpeFile::file_end() { m_file << "</ipe>\n"; }

void IpeFile::page_start() {
  m_file << "<page>\n";
  m_file << "<layer name=\"alpha\"/>\n";
  m_file << "<view layers=\"alpha\" active=\"alpha\"/>\n";
}

void IpeFile::page_end() { m_file << "</page>\n"; }

void IpeFile::normalize_point(double& x, double& y) {
  x = x * m_scaling + m_margin;
  y = y * m_scaling + m_margin;
}

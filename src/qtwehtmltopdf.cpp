/****************************************************************************
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * QT Web Engine HTML to PDF
 *
*****************************************************************************/


#include <QApplication>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QCommandLineParser>
#include <QByteArray>
#include <QDir>
#include <iostream>

using namespace std;
using namespace std::placeholders;

class Html2PdfConverter: public QObject
{
    Q_OBJECT
    public:
        explicit Html2PdfConverter(QString input_file_path, QString output_file_path="",
                                   double margin_left=20.0, double margin_top=5.0,
                                   double margin_right=20.0, double margin_bottom=5.0
                                  );
        int run();
    private slots:
        void load_finished(bool ok);
        void pdf_printing_finished(const QString &filePath, bool success);
    private:
        QString input_file_path;
        QString output_file_path;
        QWebEngineProfile profile;
        QScopedPointer<QWebEnginePage> page;
        enum page_orientation {Portrait, Landscape};
        double margin_left;
        double margin_top;
        double margin_right;
        double margin_bottom;
        void static pdf_printing_to_mem_resultCallback(const QByteArray &result_array);
};

Html2PdfConverter::Html2PdfConverter(QString input_file_path, QString output_file_path,
                                     double margin_left, double margin_top,
                                     double margin_right, double margin_bottom
                                     ):
    input_file_path(move(input_file_path))
  , output_file_path(move(output_file_path))
  , profile(new QWebEngineProfile())
  , page(new QWebEnginePage(&profile, Q_NULLPTR))
  , margin_left(move(margin_left))
  , margin_top(move(margin_top))
  , margin_right(move(margin_right))
  , margin_bottom(move(margin_bottom))
{
    profile.setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    connect(page.data(), &QWebEnginePage::loadFinished, this, &Html2PdfConverter::load_finished);
    connect(page.data(), &QWebEnginePage::pdfPrintingFinished, this, &Html2PdfConverter::pdf_printing_finished);
}

int Html2PdfConverter::run()
{
    page->load(QUrl::fromUserInput(input_file_path, QDir::currentPath()));
    return QApplication::exec();
}

void Html2PdfConverter::load_finished(bool ok)
{
    if (!ok) {
        QTextStream(stderr) << tr("failed to load '%1'").arg(input_file_path) << "\n";
        QCoreApplication::exit(1);
        return;
    }
    QPageLayout page_layout;
    page_layout = QPageLayout(QPageSize(QPageSize::A4),
                              QPageLayout::Portrait,
                              QMarginsF(margin_left, margin_top, margin_right, margin_bottom),
                              QPageLayout::Millimeter);
    if (output_file_path != ""){
        page->printToPdf(output_file_path, page_layout);
    } else {
        page->printToPdf(&this->pdf_printing_to_mem_resultCallback, page_layout);
    }
}

void Html2PdfConverter::pdf_printing_finished(const QString &file_path, bool success)
{
    if (success) {
        QCoreApplication::quit();
    } else {
        QTextStream(stderr) << tr("failed to print to output file '%1'").arg(file_path) << "\n";
        QCoreApplication::exit(1);
    }
}

void Html2PdfConverter::pdf_printing_to_mem_resultCallback(const QByteArray &result_array)
{
    std::cout << result_array.toStdString() << std::endl;
    QCoreApplication::exit(1);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("qtwehtmltopdf");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("main", "Converts the html file INPUT to the PDF file OUTPUT."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(
        QCoreApplication::translate("main", "Input"),
        QCoreApplication::translate("main", "Input URL of filename to be converted to PDF."));

    QCommandLineOption output_filename_option("output", QCoreApplication::translate("main", "Output file; pass blank for stdout"), "output", "");
    QCommandLineOption left_margin_option("margin-left", QCoreApplication::translate("main", "Left margin [mm] (default 20mm)"), "margin-left", "20");
    QCommandLineOption top_margin_option("margin-top", QCoreApplication::translate("main", "Top margin [mm] (default 5mm)"), "margin-top", "5");
    QCommandLineOption right_margin_option("margin-right", QCoreApplication::translate("main", "Right margin [mm] (default 20mm)"), "margin-right", "20");
    QCommandLineOption bottom_margin_option("margin-bottom", QCoreApplication::translate("main", "Bottom margin [mm] (default 5mm)"), "margin-bottom", "5");
    parser.addOption(output_filename_option);
    parser.addOption(left_margin_option);
    parser.addOption(top_margin_option);
    parser.addOption(right_margin_option);
    parser.addOption(bottom_margin_option);

    parser.process(app);
    const QStringList requiredArguments = parser.positionalArguments();

    if (requiredArguments.size() != 1)
        parser.showHelp(1);

    Html2PdfConverter converter(requiredArguments.at(0),
                                parser.value(output_filename_option),
                                parser.value(left_margin_option).toDouble(),
                                parser.value(top_margin_option).toDouble(),
                                parser.value(right_margin_option).toDouble(),
                                parser.value(bottom_margin_option).toDouble());
    return converter.run();
}
#include "qtwehtmltopdf.moc"

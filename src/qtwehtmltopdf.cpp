/****************************************************************************
 * QT Web Engine HTML to PDF
 *
 * This file is inspired by the examples of the Qt Toolkit.
*****************************************************************************/


#include <QApplication>
#include <QWebEnginePage>
#include <QCommandLineParser>
#include <QDir>

using namespace std;
using namespace std::placeholders;

class Html2PdfConverter: public QObject
{
    Q_OBJECT
    public:
        explicit Html2PdfConverter(QString input_file_path, QString output_file_path,
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
        QScopedPointer<QWebEnginePage> page;
        enum page_orientation {Portrait, Landscape};
        double margin_left;
        double margin_top;
        double margin_right;
        double margin_bottom;
};

Html2PdfConverter::Html2PdfConverter(QString input_file_path, QString output_file_path,
                                     double margin_left, double margin_top,
                                     double margin_right, double margin_bottom
                                     ):
    input_file_path(move(input_file_path))
  , output_file_path(move(output_file_path))
  , page(new QWebEnginePage)
  , margin_left(move(margin_left))
  , margin_top(move(margin_top))
  , margin_right(move(margin_right))
  , margin_bottom(move(margin_bottom))
{
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
    page->printToPdf(output_file_path, page_layout);
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
    parser.addPositionalArgument(
        QCoreApplication::translate("main", "Output"),
        QCoreApplication::translate("main", "Output file name."));
    QCommandLineOption left_margin_option("margin-left", QCoreApplication::translate("main", "Left margin [mm] (default 20mm)"), "margin-left", "20");
    QCommandLineOption top_margin_option("margin-top", QCoreApplication::translate("main", "Top margin [mm] (default 5mm)"), "margin-top", "5");
    QCommandLineOption right_margin_option("margin-right", QCoreApplication::translate("main", "Right margin [mm] (default 20mm)"), "margin-right", "20");
    QCommandLineOption bottom_margin_option("margin-bottom", QCoreApplication::translate("main", "Bottom margin [mm] (default 5mm)"), "margin-bottom", "5");
    parser.addOption(left_margin_option);
    parser.addOption(top_margin_option);
    parser.addOption(right_margin_option);
    parser.addOption(bottom_margin_option);

    parser.process(app);
    const QStringList requiredArguments = parser.positionalArguments();

    if (requiredArguments.size() != 2)
        parser.showHelp(1);

    Html2PdfConverter converter(requiredArguments.at(0), requiredArguments.at(1),
                                parser.value(left_margin_option).toDouble(),
                                parser.value(top_margin_option).toDouble(),
                                parser.value(right_margin_option).toDouble(),
                                parser.value(bottom_margin_option).toDouble());
    return converter.run();
}
#include "qtwehtmltopdf.moc"

#pragma once
#include <QTextDocument>
#include <QImage>
#include <QMap>
#include <QObject>
#include <QPaintDevice>
#include <QPainter>
#include <QPdfWriter>
#include <QPrinter>
#include <QPrinterInfo>

class Printer {
private:
    qlonglong            mPageNo;
    QPainter*            mPainter     { nullptr };
    QPdfWriter*          mPdf         { nullptr };
    QPrinter*            mPrinter     { nullptr };
    qreal                mXFactor;
    qreal                mYFactor;

    const QPageLayout::Unit toPdfUnits(QPrinter::Unit unit) const {
        switch(unit) {
        case QPrinter::Millimeter:  return QPageLayout::Millimeter;
        case QPrinter::Point:       return QPageLayout::Point;
        case QPrinter::Inch:        return QPageLayout::Inch;
        case QPrinter::Pica:        return QPageLayout::Pica;
        case QPrinter::Didot:       return QPageLayout::Didot;
        case QPrinter::Cicero:      return QPageLayout::Cicero;
        case QPrinter::DevicePixel: return QPageLayout::Point;
        }
        return QPageLayout::Point;
    }

    const QRectF pdfPageRect(QPrinter::Unit unit) const {
        if (unit == QPrinter::DevicePixel) return mPdf->pageLayout().paintRectPixels(mPdf->resolution());
        return mPdf->pageLayout().paintRect(toPdfUnits(unit));
    }

public:
    Printer(QPrinter::PrinterMode mode = QPrinter::ScreenResolution)
        : mPrinter(new QPrinter(mode)) { }
    Printer(const QPrinterInfo& printer, QPrinter::PrinterMode mode = QPrinter::ScreenResolution)
        : mPrinter(new QPrinter(printer, mode)) { }
    Printer(const QString& filename)
        : mPdf(new QPdfWriter(filename)) { }
    Printer(QPrinter* printer)
        : mPrinter(printer) { }
    virtual ~Printer() { delete mPainter; }

    void print();

    static constexpr qreal PointsPerInch    { 72.0 };
    static constexpr bool  WithAlignment    { true };
    static constexpr bool  WithoutAlignment { false };

    void                     newPage()                                      { if (mPrinter) mPrinter->newPage(); else mPdf->newPage(); }
    QPageLayout::Orientation pageOrientation()                              { if (mPrinter) return mPrinter->pageLayout().orientation();
                                                                              else return mPdf->pageLayout().orientation(); }
    const QRectF             pageRect(QPrinter::Unit units) const           { return mPrinter ? mPrinter->pageRect(units) : pdfPageRect(units); }
    QPaintDevice*            paintdevice()                                  { return mPrinter ? dynamic_cast<QPaintDevice*>(mPrinter) : dynamic_cast<QPaintDevice*>(mPdf); }
    QPainter*                painter()                                      { return mPainter ? mPainter : (mPainter = (mPrinter ? new QPainter(mPrinter) : new QPainter(mPdf))); }
    const qlonglong          physicalDpiX() const                           { return mPrinter ? mPrinter->logicalDpiX() : mPdf->logicalDpiX(); }
    const qlonglong          physicalDpiY() const                           { return mPrinter ? mPrinter->logicalDpiY() : mPdf->logicalDpiY(); }
    QPrinter*                qprinter()                                     { return mPrinter; }
    QPdfWriter*              qpdfwriter()                                   { return mPdf; }
    void                     setFullPage(bool to)                           { if (mPrinter) mPrinter->setFullPage(to); }
    void                     setPageOrientation(QPageLayout::Orientation o) { if (mPrinter) mPrinter->setPageOrientation(o); else mPdf->setPageOrientation(o); }
    void                     setOutputFileName(const QString& n)            { if (mPrinter) mPrinter->setOutputFileName(n); }
    void                     setOutputFormat(QPrinter::OutputFormat f)      { if (mPrinter) mPrinter->setOutputFormat(f); }
    void                     setPageSize(QPageSize::PageSizeId mPid)        { if (mPdf) mPdf->setPageSize(mPid); }
    void                     setPageMargins(const QMarginsF& m)             { if (mPdf) mPdf->setPageMargins(m, QPageLayout::Inch); }
    void                     setPrinterName(const QString& p)               { if (mPrinter) mPrinter->setPrinterName(p); }
    void                     setResolution(const int r)                     { if (mPdf) mPdf->setResolution(r); }

    const QRectF paperRect(QPrinter::Unit units) const {
        if (mPrinter) return mPrinter->paperRect(units);
        if (units == QPrinter::DevicePixel) return mPdf->pageLayout().fullRectPixels(mPdf->resolution());
        return mPdf->pageLayout().fullRect(toPdfUnits(units));
    }
};

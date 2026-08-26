#include "printdialog.h"
#include "ui_printdialog.h"

#include <QScroller>

#include "sheet.h"

PrintDialog::PrintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintDialog) {
    ui->setupUi(this);

    mSaveChanged = Sheet::ref().isChanged();

    const QRect avail = screen()->availableGeometry();
#ifdef Q_OS_ANDROID
    for (auto* combo: findChildren<QComboBox*>()) {
        auto* view = combo->view();
        QScroller::grabGesture(view->viewport(), QScroller::LeftMouseButtonGesture);
        view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    }
    for (auto* scroll: findChildren<QScrollArea*>()) QScroller::grabGesture(scroll->viewport(), QScroller::TouchGesture);
    for (auto* table: findChildren<QTableWidget*>()) {
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        QScroller::grabGesture(table->viewport(), QScroller::TouchGesture);
    }

    const int margin = 8;
    QSize wanted(avail.width()  - margin * 2,
                     avail.height() - margin * 2);

    setMinimumSize(wanted);
    resize(wanted);

    move(avail.left() + margin,
         avail.top()  + margin);

#else
    if (sizeHint().height() > avail.height()) {
        resize(width(), avail.height());
        move(pos().x(), 0);
        return;
    }
    if (pos().y() + sizeHint().height() > avail.height()) move(pos().x(), (avail.height() - sizeHint().height()) / 2);
#endif

    mPrinter = new Printer(QPrinter::HighResolution);
    if (mPrinter == nullptr || mPrinter->qprinter() == nullptr) {
        Statement("Unable to talk to the preview printer");
        reject();
        return;
    }

    mPrinterList = QPrinterInfo::availablePrinters();
    QStringList printers;
    for (QPrinterInfo& info: mPrinterList) printers << info.printerName();
    ui->printerComboBox->addItems(printers);

    QPrinterInfo def = QPrinterInfo::defaultPrinter();
    if (!def.isNull()) {
        ui->printerComboBox->setCurrentText(def.printerName());
        mDefPrinter = def.printerName();
    }

    mPrinter = new Printer(QPrinter::ScreenResolution);
    if (mPrinter == nullptr || mPrinter->qprinter() == nullptr) {
        Statement("Unable to talk to the preview printer");
        reject();
        return;
    }

    QPageSize size = getSizes(mPrinter, def);
    if (!size.isValid()) return;

    QString name = size.name();
    ui->pageSizeComboBox->setCurrentText(name);

    auto* layout = new QVBoxLayout(ui->previewFrame);
    ui->previewFrame->setLayout(layout);
    mPreview = new QPrintPreviewWidget(mPrinter->qprinter(), ui->previewFrame);
    layout->addWidget(mPreview);

    mPrintButton = new QPushButton(this);
    mPrintButton->setText("Print");
    ui->buttonBox->addButton(mPrintButton, QDialogButtonBox::NoRole);

    connect(ui->printerComboBox,     &QComboBox::currentIndexChanged, this, &PrintDialog::printerChanged);
    connect(ui->pageSizeComboBox,    &QComboBox::currentIndexChanged, this, &PrintDialog::paperChanged);
    connect(ui->orientationComboBox, &QComboBox::currentIndexChanged, this, &PrintDialog::orientationChanged);

    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, [this]() { reject(); });

    connect(mPrintButton, &QPushButton::clicked, this, [&, this]() {
        if (mPrinter == nullptr || mPrinter->qprinter() == nullptr) return;
        mPrintButton->setDisabled(true);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        mPreview->print();
        QApplication::restoreOverrideCursor();
        mPrintButton->setEnabled(true);
        accept();
    });

    auto orient = orientation();
    connect(mPreview, &QPrintPreviewWidget::paintRequested, this,
            [&, this](QPrinter* printOn) {
                if (mPrinter == nullptr || printOn == nullptr) return;
                Printer print(printOn);
                QPrinterInfo info(*printOn);
                auto pageSize = info.defaultPageSize();
                print.setPageSize(pageSize.id());
                orient = orientation();
                print.setPageOrientation(orient);
                Sheet::ref().printCharacter(&print);
            });

    updatePreview();

    Sheet::ref().fixButtonBox(ui->buttonBox);
    mReady = true;
}

void PrintDialog::updatePreview() {
    if (mPreview && mReady) mPreview->updatePreview();
}

QPageSize PrintDialog::getSizes(Printer* p, QPrinterInfo& def) {
    int idxPid = -1;
    int first = -1;
    QStringList pageSizes;
    for (int id = QPageSize::Letter; id <= QPageSize::LastPageSize; ++id) {
        QPageSize::PageSizeId pid = static_cast<QPageSize::PageSizeId>(id);
        QPageSize size(pid);
        QString name = size.name();
        if (size.isValid() && ((name.startsWith("A") && name[1].isDigit()) ||
                               name.startsWith("Letter") ||
                               name.startsWith("Legal"))) {
            if (idxPid < 0 && name.startsWith("Letter")) idxPid = size.id();
            if (first < 0) first = size.id();
            pageSizes.append(size.name());
        }
    }
    if (idxPid < 0) {
        if (first < 0) {
            static QPageSize empty;
            Statement("No pages sizes for the " + def.printerName() + ".\nCan't print without a page size.");
            return empty;
        }
        idxPid = first;
    }
    ui->printerComboBox->setCurrentText(def.printerName());

    pageSizes.sort();
    ui->pageSizeComboBox->addItems(pageSizes);
    QPageSize::PageSizeId pid = static_cast<QPageSize::PageSizeId>(idxPid);
    QPageSize size(pid);
    p->qprinter()->setPageSize(size);
    return size;
}

void PrintDialog::printerChanged(int idx) {
    auto info = QPrinterInfo::printerInfo(ui->printerComboBox->currentText());
    Printer* p = new Printer(info, QPrinter::HighResolution);
    if (p == nullptr) return;
    if (mPrinter) delete mPrinter;
    mPrinter = p;

    auto pageSize = getSizes(p, info);
    mPrinter->setPageSize(pageSize.id());

    QPageLayout::Orientation orientation = QPageLayout::Portrait;
    if (ui->orientationComboBox->currentIndex() == 1) orientation = QPageLayout::Landscape;
    mPrinter->setPageOrientation(orientation);
    updatePreview();
}

void PrintDialog::paperChanged(int) {
    if (!mPrinter) return;
    auto pageIdx = ui->pageSizeComboBox->currentIndex();
    int inc = 0;
    QPageSize::PageSizeId idx = QPageSize::Letter;
    for (int id = QPageSize::Letter; id <= QPageSize::LastPageSize; ++id) {
        QPageSize::PageSizeId pid = static_cast<QPageSize::PageSizeId>(id);
        QPageSize size(pid);
        QString name = size.name();
        if (size.isValid() && ((name.startsWith("A") && name[1].isDigit()) ||
                               name.startsWith("Letter") ||
                               name.startsWith("Legal"))) {
            if (inc == pageIdx) {
                idx = pid;
                break;
            }
        }
    }
    mPrinter->setPageSize(idx);

    QPageLayout::Orientation orientation = QPageLayout::Portrait;
    if (ui->orientationComboBox->currentIndex() == 1) orientation = QPageLayout::Landscape;
    mPrinter->setPageOrientation(orientation);
    updatePreview();
}

void PrintDialog::orientationChanged(int) {
    QPageLayout::Orientation orientation = QPageLayout::Portrait;
    if (ui->orientationComboBox->currentIndex() == 1) orientation = QPageLayout::Landscape;
    mPrinter->setPageOrientation(orientation);
    updatePreview();
}

PrintDialog::~PrintDialog() {
    Sheet::ref().setChanged(mSaveChanged);

    delete ui;
}

QPageLayout::Orientation PrintDialog::orientation() {
    return ui->orientationComboBox->currentIndex() < 2 ? QPageLayout::Orientation::Portrait : QPageLayout::Orientation::Landscape;
}

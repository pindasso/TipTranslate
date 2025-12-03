#include "OcrWorker.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

static PIX* qimageToPix(const QImage& src) {
    QImage img = src.convertToFormat(QImage::Format_RGBA8888);
    PIX* pix = pixCreate(img.width(), img.height(), 32);
    l_uint32* data = pixGetData(pix);
    int wpl = pixGetWpl(pix);
    for (int y = 0; y < img.height(); ++y) {
        const uchar* line = img.constScanLine(y);
        memcpy((uchar*)(data + y*wpl), line, img.bytesPerLine());
    }
    return pix;
}

struct PixDeleter {
    void operator()(PIX* p) const {
        if (p) {
            pixDestroy(&p);
        }
    }
};


OcrWorker::OcrWorker(QObject* p): QObject(p) {}

void OcrWorker::setLang(const QString& lang) { lang_ = lang; }


void OcrWorker::runOcr(const QImage& img) {
    tesseract::TessBaseAPI api;

    const char* datapath = "C:\\vcpkg\\installed\\x64-windows\\share\\tessdata";

    if (api.Init(datapath, lang_.toUtf8().constData()) != 0) {
        emit ocrDone("[ERRO] Falha ao iniciar Tesseract");
        return;
    }

    std::unique_ptr<PIX, PixDeleter> pix(qimageToPix(img));
    api.SetImage(pix.get());

    char* out = api.GetUTF8Text();
    QString result = QString::fromUtf8(out ? out : "");
    if (out) delete [] out;

    emit ocrDone(result);
}



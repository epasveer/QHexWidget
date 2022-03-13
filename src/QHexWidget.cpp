#include "QHexWidget.h"
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QClipboard>
#include <QtCore/QSize>
#include <QtCore/QDebug>
#include <stdexcept>

static unsigned char ebcdicToAscii (unsigned char byte) {

    static const unsigned char ebcdicToAsciiTable[256] = {
        46,  46,  46,  46,  46,  46,  46,  46, // 0
        46,  46,  46,  46,  46,  46,  46,  46, // 8
        46,  46,  46,  46,  46,  46,  46,  46, // 16
        46,  46,  46,  46,  46,  46,  46,  46, // 24
        46,  46,  46,  46,  46,  46,  46,  46, // 32
        46,  46,  46,  46,  46,  46,  46,  46, // 40
        46,  46,  46,  46,  46,  46,  46,  46, // 48
        46,  46,  46,  46,  46,  46,  46,  46, // 56
        32,  46,  46,  46,  46,  46,  46,  46, // 64
        46,  46,  91,  46,  60,  40,  43,  33, // 72
        38,  46,  46,  46,  46,  46,  46,  46, // 80
        46,  46,  33,  36,  42,  41,  59,  94, // 88
        45,  47,  46,  46,  46,  46,  46,  46, // 96
        46,  46, 124,  44,  37,  95,  62,  63, // 104
        46,  46,  46,  46,  46,  46,  46,  46, // 112
        46,  96,  58,  35,  64,  39,  61,  34, // 120
        46,  97,  98,  99, 100, 101, 102, 103, // 128
        104, 105,  46,  46,  46,  46,  46,  46, // 136
        46, 106, 107, 108, 109, 110, 111, 112, // 144
        113, 114,  46,  46,  46,  46,  46,  46, // 152
        46, 126, 115, 116, 117, 118, 119, 120, // 160
        121, 122,  46,  46,  46,  46,  46,  46, // 168
        46,  46,  46,  46,  46,  46,  46,  46, // 176
        46,  46,  46,  46,  46,  46,  46,  46, // 184
        123,  65,  66,  67,  68,  69,  70,  71, // 192
        72,  73,  46,  46,  46,  46,  46,  46, // 200
        125,  74,  75,  76,  77,  78,  79,  80, // 208
        81,  82,  46,  46,  46,  46,  46,  46, // 216
        92,  46,  83,  84,  85,  86,  87,  88, // 224
        89,  90,  46,  46,  46,  46,  46,  46, // 232
        48,  49,  50,  51,  52,  53,  54,  55, // 240
        56,  57,  46,  46,  46,  46,  46,  46  // 248
    };

    return ebcdicToAsciiTable[byte];
}

static unsigned char ucharToAscii (unsigned char byte) {

    static const unsigned char ucharToAsciiTable[256] = {
        46,  46,  46,  46,  46,  46,  46,  46, // 0
        46,  46,  46,  46,  46,  46,  46,  46, // 8
        46,  46,  46,  46,  46,  46,  46,  46, // 16
        46,  46,  46,  46,  46,  46,  46,  46, // 24
        32,  33,  34,  35,  36,  37,  38,  39, // 32
        40,  41,  42,  43,  44,  45,  46,  47, // 40
        48,  49,  50,  51,  52,  53,  54,  55, // 48
        56,  57,  58,  59,  60,  61,  62,  63, // 56
        64,  65,  66,  67,  68,  69,  70,  71, // 64
        72,  73,  74,  75,  76,  77,  78,  79, // 72
        80,  81,  82,  83,  84,  85,  86,  87, // 80
        88,  89,  90,  91,  92,  93,  94,  95, // 88
        96,  97,  98,  99, 100, 101, 102, 103, // 96
        104, 105, 106, 107, 108, 109, 110, 111, // 104
        112, 113, 114, 115, 116, 117, 118, 119, // 112
        120, 121, 122, 123, 124, 125, 126,  46, // 120
        46,  46,  46,  46,  46,  46,  46,  46, // 128
        46,  46,  46,  46,  46,  46,  46,  46, // 136
        46,  46,  46,  46,  46,  46,  46,  46, // 144
        46,  46,  46,  46,  46,  46,  46,  46, // 152
        46,  46,  46,  46,  46,  46,  46,  46, // 160
        46,  46,  46,  46,  46,  46,  46,  46, // 168
        46,  46,  46,  46,  46,  46,  46,  46, // 176
        46,  46,  46,  46,  46,  46,  46,  46, // 184
        46,  46,  46,  46,  46,  46,  46,  46, // 192
        46,  46,  46,  46,  46,  46,  46,  46, // 200
        46,  46,  46,  46,  46,  46,  46,  46, // 208
        46,  46,  46,  46,  46,  46,  46,  46, // 216
        46,  46,  46,  46,  46,  46,  46,  46, // 224
        46,  46,  46,  46,  46,  46,  46,  46, // 232
        46,  46,  46,  46,  46,  46,  46,  46, // 240
        46,  46,  46,  46,  46,  46,  46,  46  // 248
    };

    return ucharToAsciiTable[byte];
}

QHexWidget::QHexWidget(QWidget* parent) : QPlainTextEdit(parent), _pdata(NULL) {

    QFont font;
    font.setFamily("monospace [Consolas]");
    font.setFixedPitch(true);
    font.setStyleHint(QFont::TypeWriter);

    setFont(font);

    _memoryMode    = QHexWidget::HexMemoryMode;
    _charMode      = QHexWidget::AsciiCharMode;
    _addressOffset = 0;
    _charWidth     = fontMetrics().horizontalAdvance(QLatin1Char('9'));
    _charHeight    = fontMetrics().height();
    _gapAddrHex    = 10; // Gap between address and hex fields.
    _gapHexAscii   = 16; // Gap between hex and ascii fields.

    setBytesPerLine(16);

    setFocusPolicy(Qt::StrongFocus);
}

QHexWidget::~QHexWidget() {

    if (_pdata) {
        delete _pdata;
    }
}

void QHexWidget::setBytesPerLine (int count) {

    // 3 = 'a0 '
    // 4 = '007 '
    // 9 = '00011010 '
    // 4 = ' 10 '
    if (memoryMode() == QHexWidget::HexMemoryMode) {
        _hexCharsPerByte = 3;
    }else if (memoryMode() == QHexWidget::OctalMemoryMode) {
        _hexCharsPerByte = 4;
    }else if (memoryMode() == QHexWidget::BinaryMemoryMode) {
        _hexCharsPerByte = 9;
    }else if (memoryMode() == QHexWidget::DecimalMemoryMode) {
        _hexCharsPerByte = 4;
    }else{
        _hexCharsPerByte = 3;
    }

    _bytesPerLine    = count;
    _hexCharsPerLine = _bytesPerLine * _hexCharsPerByte - 1;
    _posAddr         = 0; // x position of address field.
    _posHex          = 12 * _charWidth + gapAddrHex(); // x position of hex field.
    _posAscii        = _posHex + hexCharsPerLine() * _charWidth + gapHexAscii(); // x position of ascii field.

    setMinimumWidth(_posAscii + (bytesPerLine() * _charWidth)); // x position after the ascii field.

    // Repaint the widget.
    create();
}

int QHexWidget::bytesPerLine () const {
    return _bytesPerLine;
}

int QHexWidget::hexCharsPerLine () const {
    return _hexCharsPerLine;
}

int QHexWidget::gapAddrHex () const {
    return _gapAddrHex;
}

int QHexWidget::gapHexAscii () const {
    return _gapHexAscii;
}

void QHexWidget::setAddressOffset (unsigned long offset) {

    _addressOffset = offset;

    // Repaint the widget.
    create();
}

unsigned long QHexWidget::addressOffset () const {
    return _addressOffset;
}

unsigned long QHexWidget::size () const {

    if (_pdata) {
        return _pdata->size();
    }

    return 0;
}

void QHexWidget::setMemoryMode (QHexWidget::MemoryMode memoryMode) {

    _memoryMode = memoryMode;

    // This repaints the widget with the new memory mode
    setBytesPerLine(bytesPerLine());
}

QHexWidget::MemoryMode QHexWidget::memoryMode () const {
    return _memoryMode;
}

QString QHexWidget::memoryModeString () const {

    if (memoryMode() == QHexWidget::HexMemoryMode) {
        return "hex";
    }else if (memoryMode() == QHexWidget::OctalMemoryMode) {
        return "octal";
    }else if (memoryMode() == QHexWidget::BinaryMemoryMode) {
        return "binary";
    }else if (memoryMode() == QHexWidget::DecimalMemoryMode) {
        return "decimal";
    }

    return "???";
}

void QHexWidget::setCharMode (QHexWidget::CharMode charMode) {

    _charMode = charMode;

    // Repaint the widget.
    create();
}

QHexWidget::CharMode QHexWidget::charMode() const {
    return _charMode;
}

QString QHexWidget::charModeString () const {

    if (charMode() == QHexWidget::AsciiCharMode) {
        return "ascii";
    }else if (charMode() == QHexWidget::EbcdicCharMode) {
        return "ebcdic";
    }

    return "???";
}

void QHexWidget::setData(QHexWidget::DataStorage* pData) {

    if (_pdata) {
        delete _pdata;
    }

    _pdata     = pData;
    _cursorPos = 0;

    // Repaint the widget.
    create();
}

void QHexWidget::create () {

    // Clear the current document. We're going to recreate it.
    clear();

    // If there's no data, do nothing.
    if (!_pdata) {
        return;
    }

    // Set text formats.
    QTextCharFormat defaultFormat = currentCharFormat();
    QTextCharFormat grayFormat    = defaultFormat;
    grayFormat.setBackground(QBrush(Qt::lightGray));

    // Get a cursor
    QTextCursor cursor(textCursor());

    cursor.movePosition(QTextCursor::Start);

    // Go through the data, one byte at a time.
    for (int i=0; i<_pdata->size(); i+=bytesPerLine()) {

        QByteArray data = _pdata->getData(i, bytesPerLine());

        // Place a new hex address on the left side.
        if (i % bytesPerLine() == 0) {

            QString address = QString("%1").arg(i + addressOffset(), 12, 16, QChar('0'));
            QString spacer(" ");

            // Write adress to document.
            cursor.insertText (address, grayFormat);

            // Write spacer to document.
            cursor.insertText (spacer, defaultFormat);
        }

        // Print N bytes in their datatype value.
        int b = 0;

        for (b=0; b<bytesPerLine() && i+b < _pdata->size(); b++) {

            unsigned char ch = data[b];

            QString val;

            if (memoryMode() == QHexWidget::HexMemoryMode) {
                val = QString("%1").arg(ushort(ch), int(2), int(16), QChar('0'));
            }else if (memoryMode() == QHexWidget::OctalMemoryMode) {
                val = QString("%1").arg(ushort(ch), int(3), int(8), QChar('0'));
            }else if (memoryMode() == QHexWidget::BinaryMemoryMode) {
                val = QString("%1").arg(ushort(ch), int(8), int(2), QChar('0'));
            }else if (memoryMode() == QHexWidget::DecimalMemoryMode) {
                val = QString("%1").arg(ushort(ch), int(3), int(10), QChar(' '));
            }else{
                val = "??";
            }

            QString spacer(" ");

            // Write memory value to document
            cursor.insertText (val, defaultFormat);

            // Write spacer to document.
            cursor.insertText (spacer, defaultFormat);
        }

        // Print N odd remainder bytes.
        for (; b<bytesPerLine(); b++) {

            QString val;

            if (memoryMode() == QHexWidget::HexMemoryMode) {
                val = "  ";
            }else if (memoryMode() == QHexWidget::OctalMemoryMode) {
                val = "   ";
            }else if (memoryMode() == QHexWidget::BinaryMemoryMode) {
                val = "        ";
            }else if (memoryMode() == QHexWidget::DecimalMemoryMode) {
                val = "   ";
            }else{
                val = "??";
            }

            QString spacer(" ");

            // Write memory value to document
            cursor.insertText (val, defaultFormat);

            // Write spacer to document.
            cursor.insertText (spacer, defaultFormat);
        }

        // Print vertical line.
        QString spacer("| ");

        // Write spacer to document.
        cursor.insertText (spacer, defaultFormat);

        // Print N bytes in their char value.
        if (charMode() == QHexWidget::AsciiCharMode) {
            for (int b=0; b<bytesPerLine() && i+b < _pdata->size(); b++) {

                unsigned char ch = ucharToAscii( data[b] );

                QString val(ch);

                // Write display character to document.
                cursor.insertText (val, defaultFormat);
            }

        }else if (charMode() == QHexWidget::EbcdicCharMode) {
            for (int b=0; b<bytesPerLine() && i+b < _pdata->size(); b++) {

                unsigned char ch = ebcdicToAscii( data[b] );

                QString val(ch);

                // Write display character to document.
                cursor.insertText (val, defaultFormat);
            }

        }else{
            // Don't print anything.
        }

        // Write eol to document.
        QString eol("\n");

        cursor.insertText (eol, defaultFormat);
    }
}

QHexWidget::DataStorageArray::DataStorageArray(const QByteArray& arr) {
    _data = arr;
}

QByteArray QHexWidget::DataStorageArray::getData(int position, int length) {
    return _data.mid(position, length);
}

int QHexWidget::DataStorageArray::size() {
    return _data.count();
}


#include "applicationdata.h"

#include <QDir>
#include <QUrl>
#include <QFile>
#include <QBuffer>
#include <QByteArray>
#include <QTextStream>

#include "ScintillaEditBase.h"

#include "ILexer.h"
#include "Scintilla.h"
#include "Lexilla.h"
#include "SciLexer.h"


ApplicationData::ApplicationData(QObject *parent)
    : QObject(parent)
{
}

ApplicationData::~ApplicationData()
{
}

void ApplicationData::setScintilla(QObject * obj)
{
    ScintillaEditBase * base = reinterpret_cast<ScintillaEditBase *>(obj);   
    Scintilla::ILexer5 *plexer = CreateLexer("python");
    base->sends(SCI_SETILEXER, 0, (const char*)plexer);
	base->sends(SCI_STYLERESETDEFAULT, 0, 0);
    base->sends(SCI_STYLESETFORE, SCE_P_COMMENTLINE, (const char*) "#ff0000");	
	base->sends(SCI_COLOURISE, 0, (const char*)-1);	
	base->update();
}

bool IsAndroidStorageFileUrl(const QString & url)
{
    return url.startsWith("content:/");
}

QString GetTranslatedFileName(const QString & fileName)
{
    QString translatedFileName = fileName;
    if( IsAndroidStorageFileUrl(fileName) )
    {
        // handle android storage urls --> forward content://... to QFile directly
        translatedFileName = fileName;
    }
    else
    {
        QUrl url(fileName);
        if(url.isValid() && url.isLocalFile())
        {
            translatedFileName = url.toLocalFile();
        }
    }
    return translatedFileName;
}

QString ApplicationData::readFileContent(const QString & fileName) const
{
    QString translatedFileName = GetTranslatedFileName(fileName);

    QFile file(translatedFileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString(tr("Error reading ") + fileName);
    }

    QTextStream stream(&file);
    auto text = stream.readAll();

    file.close();

    return text;
}

bool ApplicationData::writeFileContent(const QString & fileName, const QString & content)
{
    QString translatedFileName = GetTranslatedFileName(fileName);

    QFile file(translatedFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream stream(&file);
    stream << content;

    file.close();

    return true;
}

bool ApplicationData::deleteFile(const QString & fileName)
{
    QFile aDir(fileName);
    bool ok = aDir.remove();
    return ok;
}

QString ApplicationData::readLog() const
{
    return readFileContent(LOG_NAME);
}

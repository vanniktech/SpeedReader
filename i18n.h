#ifndef I18N_H
#define I18N_H

#include <QObject>
#include <QTranslator>

struct Language {
    QString name;
    QString iconFilePath;
    QString locale;
    bool isDefault;
};

class I18N : public QObject
{
    Q_OBJECT
public:
    explicit I18N(QObject *parent = 0);

    QList<Language> getLanguages();
    QString getDefaultLocale();
    QString getBase64GooglePlayStoreIconForCurrentLanguage();

    void loadLanguage(const QString& language);
    void onSystemLocalChanged();

signals:

public slots:
private:
    QTranslator mTranslator;
    QString     mCurrentLanguage;

    void createLanguageMenu();
    void switchTranslator(QTranslator& translator, const QString& filename);
};

#endif // I18N_H

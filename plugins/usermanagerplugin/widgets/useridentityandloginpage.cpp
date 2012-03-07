#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/translators.h>


#include "useridentityandloginpage.h"
#include "ui_useridentityandloginpage.h"

#include <utils/log.h>
#include <utils/global.h>
#include <listviewplugin/languagecombobox.h>
#include "utils/widgets/uppercasevalidator.h"
#include <translationutils/constanttranslations.h>

#include "coreplugin/translators.h"

#include <usermanagerplugin/usermodel.h>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline UserPlugin::UserModel *userModel() { return UserModel::instance(); }

UserIdentityAndLoginPage::UserIdentityAndLoginPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::UserIdentityAndLoginPage)
{
    ui->setupUi(this);

    ui->cbLanguage->setDisplayMode(Views::LanguageComboBox::AvailableTranslations);
    ui->cbLanguage->setCurrentLanguage(QLocale().language());

    Utils::UpperCaseValidator *val = new Utils::UpperCaseValidator(this);
    ui->leName->setValidator(val);
    ui->leSecondName->setValidator(val);

    connect(ui->cbLanguage, SIGNAL(currentLanguageChanged(QLocale::Language)), Core::Translators::instance(), SLOT(changeLanguage(QLocale::Language)));

    registerField("Language", ui->cbLanguage , "currentLanguage");
    registerField("Name*", ui->leName);
    registerField("Firstname*", ui->leFirstName);
    registerField("SecondName", ui->leSecondName);
    registerField("Title", ui->cbTitle);
    registerField("Gender", ui->cbGender);

    // TODO: centralize login/password length with a constant!
    ui->leLogin->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]{6,}"), this));
    ui->leLogin->setToolTip(tr("minimum: 6 characters\nonly characters and digits allowed"));

    // TODO: centralize login/password length with a constant!
    ui->lePassword->lineEdit()->setValidator(new QRegExpValidator(QRegExp(".{6,}"),this));
    ui->lePassword->lineEdit()->setToolTip(tr("minimum: 6 characters"));
    ui->lePassword->toogleEchoMode();
    ui->lePassword->setIcon(theme()->icon(Core::Constants::ICONEYES));

    ui->lePasswordConfirm->toogleEchoMode();
    ui->lePasswordConfirm->setIcon(theme()->icon(Core::Constants::ICONEYES));

    registerField("Login*", ui->leLogin);
    registerField("Password*", ui->lePassword, "text");
    registerField("ConfirmPassword*", ui->lePasswordConfirm, "text");

    retranslate();

    connect(ui->leLogin, SIGNAL(editingFinished()), this, SLOT(checkLogin()));
    connect(ui->lePasswordConfirm->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(checkControlPassword(QString)));
    connect(ui->lePassword->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(checkControlPassword(QString)));

    // set right stylesheets to the labels
    checkControlPassword("");
    on_leName_textChanged("");
    on_leFirstName_textChanged("");
    on_leLogin_textChanged("");
}

UserIdentityAndLoginPage::~UserIdentityAndLoginPage()
{
    delete ui;
}

void UserIdentityAndLoginPage::checkLogin()
{
    //TODO: code here

    // user login must be unique in the FreeMedForms database
    // user login must be unique on the server
}

void UserIdentityAndLoginPage::checkControlPassword(const QString &text)
{
    Q_UNUSED(text);

    QString stylesheet = "color:red;";
    // TODO: centralize login/password length with a constant!

    // check wether both passwords meet the specifications
    if (ui->lePassword->text().length() >= 6 && ui->lePasswordConfirm->text().length() >= 6) {

        if (ui->lePasswordConfirm->text() == ui->lePassword->text()) { // congruent passwords
            stylesheet = "";
        }
    }
    ui->lblConfirmPassword->setStyleSheet(stylesheet);
    ui->lblPassword->setStyleSheet(stylesheet);
}

void UserIdentityAndLoginPage::on_leName_textChanged(const QString &text)
{
        ui->lblName->setStyleSheet(!text.isEmpty()? 0 : "color:red;");
}

void UserIdentityAndLoginPage::on_leFirstName_textChanged(const QString &text)
{
        ui->lblFirstName->setStyleSheet(!text.isEmpty()? 0 : "color:red;");
}

void UserIdentityAndLoginPage::on_leLogin_textChanged(const QString &text)
{
        ui->lblLogin->setStyleSheet(text.length() >= 6? 0 : "color:red;");
}

void UserIdentityAndLoginPage::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
        retranslate();
}

void UserIdentityAndLoginPage::retranslate()
{
    setTitle(tr("Create a new user."));
    setSubTitle(tr("Enter your identity."));

    /* TODO: the following lines are not really needed
      as in the autocreated ui_ file the retranslate() method
      is already implemented, but that method just doesn't use the
      Trans::Constants::XXXX strings we should use here */

    if (ui->langLbl) {
        ui->langLbl->setText(tr("Language"));
        ui->lblTitle->setText(tkTr(Trans::Constants::TITLE));
        ui->lblName->setText(tkTr(Trans::Constants::NAME));
        ui->lblFirstName->setText(tr("First name"));
        ui->lblSecondName->setText(tr("Second name"));
        ui->lblGender->setText(tkTr(Trans::Constants::GENDER));
        ui->cbTitle->addItems(titles());
        ui->cbGender->addItems(genders());
        ui->lblLogin->setText(tkTr(Trans::Constants::LOGIN));
        ui->lblPassword->setText(tkTr(Trans::Constants::PASSWORD));
        ui->lblConfirmPassword->setText(tr("Confirm password"));
        ui->identGroup->setTitle(tr("Identity"));
        ui->logGroup->setTitle(tr("Database connection"));
    }
}


bool UserIdentityAndLoginPage::validatePage()
{
    if (field("Name").toString().isEmpty() || field("Firstname").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Forbidden anonymous user."),
                                 tr("All users must have at least a name and a first name.\n"
                                    "You can not proceed with an anonymous user."), "",
                                 tr("Forbidden anonymous user."));
        return false;
    }
    if (field("Password").toString() != field("ConfirmPassword")) {
        Utils::warningMessageBox(tr("Password confirmation error."),
                                 tr("You must correctly confirm your password to go through this page."),
                                 "", tr("Wrong password"));
        return false;
    }
    if (field("Login").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Login error."),
                                 tr("You must specify a valid login. Empty login is forbidden."),
                                 "", tr("Wrong login"));
        return false;
    }
    if (field("Login").toString().size() < 6) {
        Utils::warningMessageBox(tr("Login error."),
                                 tr("You must specify a valid login. Login must be more than 6 characters."),
                                 "", tr("Wrong login"));
        return false;
    }
    // log/pass already used ?
    if (userModel()->isCorrectLogin(field("Login").toString(),
                                    field("Password").toString())) {
        Utils::warningMessageBox(tr("Login and password already used"),
                                 tr("The users' database already contains the same login/password couple.\n"
                                    "You must specify a different login/password."),
                                 "", tr("Login/Password already used"));
        return false;
    }
    return true;
}

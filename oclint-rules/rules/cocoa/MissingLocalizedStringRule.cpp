#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/util/StdUtil.h"

using namespace std;
using namespace clang;
using namespace oclint;

class MissingLocalizedStringRule : public AbstractASTVisitorRule<MissingLocalizedStringRule>
{
private:
    static RuleSet rules;

    bool isLocalizedStringGetSelector(Expr *expr, string &selectorString)
    {
        return expr->getType().getAsString() == "NSBundle *" &&
            selectorString == "localizedStringForKey:value:table:";
    }

    bool isLocalizedStringKeyMissing(string &localizedStringKey)
    {
        // TODO: Return whether or not the localizedStringKey is missing 
        // from Localizable.strings
        return false;
    }

public:
    virtual const string name() const
    {
        return "missing localized string";
    }

    virtual int priority() const
    {
        return 1;
    }

    virtual unsigned int supportedLanguages() const
    {
        return LANG_OBJC;
    }

    bool VisitObjCMessageExpr(ObjCMessageExpr *objCMsgExpr)
    {
        Expr *receiverExpr = objCMsgExpr->getInstanceReceiver();
        string selectorString = objCMsgExpr->getSelector().getAsString();
        string localizedStringKey = objCMsgExpr->getArg(0).getAsString();

        if (receiverExpr && isa<ImplicitCastExpr>(receiverExpr) &&
            isLocalizedStringGetSelector(receiverExpr, selectorString) &&
            isLocalizedStringKeyMissing(localizedStringKey))
        {
            addViolation(objCMsgExpr, this);
        }

        return true;
    }

};

RuleSet MissingLocalizedStringRule::rules(new MissingLocalizedStringRule());

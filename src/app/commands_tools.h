/****************************************************************************
** Copyright (c) 2022, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
** See license at https://github.com/fougue/mayo/blob/master/LICENSE.txt
****************************************************************************/

#pragma once

#include "commands_api.h"

class QJSEngine;

namespace Mayo {

class CommandSaveViewImage : public Command {
public:
    CommandSaveViewImage(IAppContext* context);
    void execute() override;
    bool getEnabledStatus() const override;

    static constexpr std::string_view Name = "save-view-image";
};

class CommandInspectXde : public Command {
public:
    CommandInspectXde(IAppContext* context);
    void execute() override;
    bool getEnabledStatus() const override;

    static constexpr std::string_view Name = "inspect-xde";
};

class CommandEditOptions : public Command {
public:
    CommandEditOptions(IAppContext* context);
    void execute() override;

    static constexpr std::string_view Name = "edit-options";
};

class CommandExecScript : public Command {
public:
    CommandExecScript(IAppContext* context);
    void execute() override;

    static constexpr std::string_view Name = "exec-script";

private:
    QJSEngine* m_jsEngine = nullptr;
};

} // namespace Mayo

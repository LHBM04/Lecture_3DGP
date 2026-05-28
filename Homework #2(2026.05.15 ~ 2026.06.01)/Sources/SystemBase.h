#pragma once

class EngineContext;

class SystemBase
{
public:
    SystemBase() = default;
    virtual ~SystemBase() = default;

    SystemBase(const SystemBase&) = delete;
    SystemBase& operator=(const SystemBase&) = delete;

    SystemBase(SystemBase&&) = delete;
    SystemBase& operator=(SystemBase&&) = delete;

    virtual std::expected<void, std::wstring> Initialize(const EngineContext& context_) = 0;
    virtual void Release() = 0;
};
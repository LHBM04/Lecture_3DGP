#pragma once

#include "InputContext.h"

class InputHandler
{
public:
	virtual ~InputHandler() = default;

	virtual void OnInputStarted(const InputContext& context_) { (void)context_; }
	virtual void OnInputPerformed(const InputContext& context_) { (void)context_; }
	virtual void OnInputCanceled(const InputContext& context_) { (void)context_; }
};

// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test/unittests/heap/cppgc/test-platform.h"

#include "include/libplatform/libplatform.h"
#include "src/base/platform/platform.h"
#include "src/base/platform/time.h"

namespace cppgc {
namespace internal {
namespace testing {

TestPlatform::TestPlatform()
    : DefaultPlatform(0, DefaultPlatform::IdleTaskSupport::kEnabled) {}

std::unique_ptr<cppgc::JobHandle> TestPlatform::PostJob(
    cppgc::TaskPriority priority, std::unique_ptr<cppgc::JobTask> job_task) {
  if (AreBackgroundTasksDisabled()) return nullptr;
  return v8_platform_->PostJob(priority, std::move(job_task));
}

void TestPlatform::RunAllForegroundTasks() {
  v8::platform::PumpMessageLoop(v8_platform_.get(), kNoIsolate);
  if (GetForegroundTaskRunner()->IdleTasksEnabled()) {
    v8::platform::RunIdleTasks(v8_platform_.get(), kNoIsolate,
                               std::numeric_limits<double>::max());
  }
}

TestPlatform::DisableBackgroundTasksScope::DisableBackgroundTasksScope(
    TestPlatform* platform)
    : platform_(platform) {
  ++platform_->disabled_background_tasks_;
}

TestPlatform::DisableBackgroundTasksScope::~DisableBackgroundTasksScope()
    V8_NOEXCEPT {
  --platform_->disabled_background_tasks_;
}

}  // namespace testing
}  // namespace internal
}  // namespace cppgc

#include <stdexcept>

#include "buffer/lru_k_replacer.h"
#include "gtest/gtest.h"

namespace bustub {

TEST(LRUKReplacerRegressionTest, KOneUsesMostRecentAccess) {
  LRUKReplacer replacer(2, 1);
  replacer.RecordAccess(0);
  replacer.RecordAccess(1);
  replacer.RecordAccess(0);
  replacer.SetEvictable(0, true);
  replacer.SetEvictable(1, true);
  frame_id_t victim = -1;
  ASSERT_TRUE(replacer.Evict(&victim));
  EXPECT_EQ(victim, 1);
}

TEST(LRUKReplacerRegressionTest, IncompleteHistoryUsesEarliestAccess) {
  LRUKReplacer replacer(2, 3);
  replacer.RecordAccess(0);
  replacer.RecordAccess(1);
  replacer.RecordAccess(0);
  replacer.SetEvictable(0, true);
  replacer.SetEvictable(1, true);
  frame_id_t victim = -1;
  ASSERT_TRUE(replacer.Evict(&victim));
  EXPECT_EQ(victim, 0);
}

TEST(LRUKReplacerRegressionTest, InvalidFramesDoNotChangeState) {
  LRUKReplacer replacer(2, 2);
  EXPECT_THROW(replacer.RecordAccess(-1), std::out_of_range);
  EXPECT_THROW(replacer.RecordAccess(2), std::out_of_range);
  EXPECT_THROW(replacer.SetEvictable(2, true), std::out_of_range);
  EXPECT_THROW(replacer.Remove(-1), std::out_of_range);
  EXPECT_EQ(replacer.Size(), 0);
  EXPECT_THROW(LRUKReplacer(2, 0), std::invalid_argument);
}

TEST(LRUKReplacerRegressionTest, EvictableChangesAreIdempotent) {
  LRUKReplacer replacer(1, 2);
  replacer.SetEvictable(0, true);
  EXPECT_EQ(replacer.Size(), 0);
  replacer.RecordAccess(0);
  replacer.SetEvictable(0, true);
  replacer.SetEvictable(0, true);
  EXPECT_EQ(replacer.Size(), 1);
  replacer.SetEvictable(0, false);
  replacer.SetEvictable(0, false);
  EXPECT_EQ(replacer.Size(), 0);
}

TEST(LRUKReplacerRegressionTest, RejectRemovalOfPinnedFrame) {
  LRUKReplacer replacer(1, 2);
  replacer.RecordAccess(0);
  EXPECT_THROW(replacer.Remove(0), std::logic_error);
  replacer.SetEvictable(0, true);
  frame_id_t victim = -1;
  ASSERT_TRUE(replacer.Evict(&victim));
  EXPECT_EQ(victim, 0);
}

TEST(LRUKReplacerRegressionTest, ReusedFrameStartsWithEmptyHistory) {
  LRUKReplacer replacer(2, 2);
  for (frame_id_t frame = 0; frame < 2; ++frame) {
    replacer.RecordAccess(frame);
    replacer.RecordAccess(frame);
    replacer.SetEvictable(frame, true);
  }
  frame_id_t victim = -1;
  ASSERT_TRUE(replacer.Evict(&victim));
  ASSERT_EQ(victim, 0);
  replacer.RecordAccess(0);
  replacer.SetEvictable(0, true);
  ASSERT_TRUE(replacer.Evict(&victim));
  EXPECT_EQ(victim, 0);
}

}  // namespace bustub

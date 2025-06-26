pub mod decision_caller;
pub mod decision_tree;

use crate::client::ZappyClient;
use async_trait::async_trait;

#[derive(Debug, Clone, PartialEq)]
pub enum Priority {
    Critical,
    High,
    Medium,
    Low,
}

#[derive(Debug, Clone)]
pub enum Action {
    FindFood,
    CollectResource,
    LevelUp,
    Explore,
    HatchEgg,
    Wait,
}

#[async_trait]
pub trait DecisionNode {
    async fn evaluate(&self, client: &mut ZappyClient) -> (Priority, Action);
}
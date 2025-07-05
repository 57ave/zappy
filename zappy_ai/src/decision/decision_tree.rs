use crate::client::ZappyClient;
use super::{Priority, Action, DecisionNode};
use async_trait::async_trait;

pub enum DecisionNodeEnum {
    Food(FoodNode),
    Resource(ResourceNode),
    LevelUp(LevelUpNode),
    Join(JoinNode),
}

pub struct DecisionTree {
    root: Box<DecisionNodeEnum>,
}

pub struct FoodNode {
    food_threshold: u32,
    next: Option<Box<DecisionNodeEnum>>,
}

pub struct ResourceNode {
    resource_priority: Vec<String>,
    next: Option<Box<DecisionNodeEnum>>,
}

pub struct LevelUpNode {
    next: Option<Box<DecisionNodeEnum>>,
}

#[async_trait]
impl DecisionNode for DecisionNodeEnum {
    async fn evaluate(&self, client: &mut ZappyClient) -> (Priority, Action) {
        match self {
            DecisionNodeEnum::Food(node) => node.evaluate(client).await,
            DecisionNodeEnum::Resource(node) => node.evaluate(client).await,
            DecisionNodeEnum::LevelUp(node) => node.evaluate(client).await,
            DecisionNodeEnum::Join(node) => node.evaluate(client).await,
        }
    }
}

#[async_trait]
impl DecisionNode for FoodNode {
    async fn evaluate(&self, client: &mut ZappyClient) -> (Priority, Action) {
        match client.see_food().await {
            Ok(food_count) if food_count < self.food_threshold => (Priority::High, Action::FindFood),
            Ok(_) => {
                if let Some(next) = &self.next {
                    next.evaluate(client).await
                } else {
                    (Priority::Critical, Action::CollectResource)
                }
            }
            Err(e) => {
                eprintln!("Error checking food: {:?}", e);
                (Priority::High, Action::LayEgg)
            }
        }
    }
}

#[async_trait]
impl DecisionNode for ResourceNode {
    async fn evaluate(&self, client: &mut ZappyClient) -> (Priority, Action) {
        let current_level = client.get_level().await.unwrap_or(1);
        let inventory = client.inventory().await.unwrap_or_default();
        if current_level < 4 {
            let level_4_req = crate::drone::levels::get_level_requirements()
                .into_iter()
                .find(|r| r.level == 4)
                .expect("Level 4 requirements should exist");
            
            let has_all = level_4_req.resources.iter()
                .all(|(resource, &count)| inventory.get_resource(resource) >= count as i32);
            if has_all {
                if let Ok(true) = client.has_level_requirements().await {
                    return (Priority::Critical, Action::LevelUp);
                }
            }
        }
        if inventory.food > 8 {
            return (Priority::High, Action::CollectResource);
        }
        if let Some(next) = &self.next {
            next.evaluate(client).await
        } else {
            (Priority::Low, Action::Explore)
        }
    }
}

#[async_trait]
impl DecisionNode for LevelUpNode {
    async fn evaluate(&self, client: &mut ZappyClient) -> (Priority, Action) {
        let current_level = match client.get_level().await {
            Ok(level) => level,
            Err(_) => return (Priority::High, Action::LayEgg),
        };

        match client.coordinate_level_up().await {
            Ok(_) => (Priority::Critical, Action::JoinTeam),
            Err(_) => {
                if let Some(next) = &self.next {
                    next.evaluate(client).await
                } else {
                    (Priority::High, Action::LayEgg)
                }
            }
        }
    }
}

pub struct JoinNode {
    next: Option<Box<DecisionNodeEnum>>,
}

#[async_trait]
impl DecisionNode for JoinNode {
    async fn evaluate(&self, client: &mut ZappyClient) -> (Priority, Action) {
        if let Ok(Some(message)) = client.check_messages().await {
            match client.does_need_help(&message).await {
                Ok(Some((target_level, _position))) => {
                    if client.should_respond_to_help(target_level).await.unwrap_or(false) {
                        return (Priority::High, Action::JoinTeam);
                    }
                }
                Ok(None) => (),
                Err(e) => {
                    if client.debug {
                        eprintln!("Error parsing help message: {:?}", e);
                    }
                }
            }
        }

        if let Some(next) = &self.next {
            next.evaluate(client).await
        } else {
            (Priority::High, Action::LayEgg)
        }
    }
}


impl DecisionTree {
    pub fn new() -> Self {
        let level_node = DecisionNodeEnum::LevelUp(LevelUpNode {
            next: None,
        });
        let resource_node = DecisionNodeEnum::Resource(ResourceNode {
            resource_priority: vec![
                "linemate".to_string(),
                "deraumere".to_string(),
                "sibur".to_string(),
                "phiras".to_string(),
            ],
            next: Some(Box::new(level_node)),
        });
        let food_node = DecisionNodeEnum::Food(FoodNode {
            food_threshold: 4,  // Critical food level
            next: Some(Box::new(resource_node)),
        });
        DecisionTree {
            root: Box::new(food_node),
        }
    }
    pub async fn evaluate(&self, client: &mut ZappyClient) -> (Priority, Action) {
            let action: (Priority, Action) = self.root.evaluate(client).await;
            println!("Decision [{:?}]: {:?}", action.0, action.1);
            action
    }
} 
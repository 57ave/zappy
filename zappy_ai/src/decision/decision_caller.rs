use std::time::Duration;

use tokio::time::sleep;

use crate::client::ZappyClient;
use crate::error::ClientError;
use crate::drone::inventory::Resource;
use super::decision_tree::DecisionTree;
use super::{Priority, Action};

pub async fn make_decision(client: &mut ZappyClient) -> Result<(), ClientError> {
    let decision_tree = DecisionTree::new();
    
    loop {
        let (priority, action) = decision_tree.evaluate(client).await;
        match (priority, action) {
            (Priority::Critical, Action::FindFood) => {
                handle_food_search(client).await?;
            },
            (Priority::High, Action::CollectResource) => {
                handle_resource_collection(client).await?;
            },
            (Priority::High, Action::LevelUp) => {
                handle_level_up(client).await?;
            },
            (_, Action::Explore) => {
                handle_exploration(client).await?;
            },
            _ => {
                client.wait().await?;
            }
        }
        sleep(Duration::from_millis(100)).await;
        client.reset_look_cache();
    }
}

async fn handle_food_search(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.get_look_cached().await?;
    
    if client.see_food().await? > 0{
        client.move_to_food().await?;
        client.take(Resource::Food).await?;
    } else {
        client.forward().await?;
    }
    
    Ok(())
}

async fn handle_resource_collection(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.get_look_cached().await?;
    
    if client.see_priority_resource().await? {
        client.move_to_resource().await?;
    }
    
    Ok(())
}

async fn handle_level_up(client: &mut ZappyClient) -> Result<(), ClientError> {
    if client.has_level_requirements().await? {
        client.incantation().await?;
    }
    Ok(())
}

async fn handle_exploration(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.get_look_cached().await?;
    client.forward().await?;
    
    Ok(())
}

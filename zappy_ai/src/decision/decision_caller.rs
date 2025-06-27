use std::time::Duration;

use tokio::time::sleep;
use rand::Rng;

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
            (Priority::High, Action::FindFood) => {
                handle_food_search(client).await?;
            },
            (Priority::High, Action::CollectResource) => {
                handle_resource_collection(client).await?;
            },
            (Priority::High, Action::LevelUp) => {
                client.handle_level_up().await?;
            },
            (_, Action::LayEgg) => {
                client.fork().await?;
            }
            (_, Action::Explore) => {
                handle_exploration(client).await?;
            },
            (_) => {
                handle_exploration(client).await?;
            },
            
        }
        if client.debug {println!("\n---");}
        sleep(Duration::from_millis(100)).await;
        client.reset_look_cache();
    }
}

async fn handle_food_search(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.get_look_cached().await?;
    
    if client.move_to_food().await? {
        client.take(Resource::Food).await?;
    } else {
        handle_exploration(client).await?;
    }
    Ok(())
}

async fn handle_resource_collection(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.get_look_cached().await?;
    
    if client.move_to_resource().await? {
        client.take_all_resources().await?;
    }
    Ok(())
}

async fn handle_exploration(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.get_look_cached().await?;
    client.take_all_resources().await?;

    let direction = rand::rng().random_range(0..3);
    
    match direction {
        0 => {
            client.forward().await?;
        },
        1 => {
            client.left().await?;
            client.forward().await?;
        },
        2 => {
            client.right().await?;
            client.forward().await?;
        },
        _ => unreachable!(),
    }

    client.reset_look_cache();
    Ok(())
}

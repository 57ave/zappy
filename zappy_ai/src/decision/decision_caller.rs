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
            (Priority::Critical, Action::LevelUp) => {
                handle_level_up(client).await?;
            },
            (Priority::High, Action::LayEgg) => {
                handle_lay_egg(client).await?;
            }
            (Priority::High, Action::JoinTeam) => {
                handle_join_team(client).await?;
            },
            (Priority::High, Action::MaintainFood) => {
                handle_maintain_food_supply(client).await?;
            },
            (Priority::Low, Action::Explore) => {
                handle_exploration(client).await?;
            },
            (_) => {
                println!("No action taken, waiting...");
                sleep(Duration::from_secs(1)).await;
            }
            
        }
        client.process_broadcasts().await?;
        client.reset_look_cache();
    }
}

pub async fn handle_lay_egg(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.fork().await?;
    println!("has laid an egg");
    Ok(())
}

pub async fn handle_level_up(client: &mut ZappyClient) -> Result<(), ClientError> {
    if client.has_level_requirements().await? {
        println!("\n\n---\nHas level up\n---\n\n\n");
        client.incantation().await?;
        client.player_state.set_level(client.player_state.get_level() + 1);
    } else {
        handle_lay_egg(client).await?;
    }
    Ok(())
}

async fn handle_food_search(client: &mut ZappyClient) -> Result<(), ClientError> {
    client.get_look_cached().await?;
    if client.move_to_food().await? {
        client.take(Resource::Food).await?;
        return Ok(());
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

pub async fn handle_maintain_food_supply(client: &mut ZappyClient) -> Result<(), ClientError> {
    let inventory = client.inventory().await?;
    if inventory.food < 5 {
        client.move_to_food().await;
        return Ok(());
    }
    Ok(())
}

pub async fn handle_join_team(client: &mut ZappyClient) -> Result<(), ClientError> {
    if let Some(message) = client.check_messages().await? {
        if let Some((target_level, position)) = client.does_need_help(&message).await? {
            if client.should_respond_to_help(target_level).await? {
                let pos = client.player_state.get_position();
                let msg = format!("RESP|{}|{}|{}|{}",
                    target_level,
                    client.team_name,
                    pos.x,
                    pos.y
                );
                client.broadcast(&msg).await?;
                
                client.move_to_position(position).await?;
                return Ok(());
            }
        }
    }
    return Ok(());
}
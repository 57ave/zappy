use crate::client::ZappyClient;
use crate::error::ClientError;
use crate::commands::Resource;
use tokio::time::{sleep, Duration};

pub async fn food_provider(client: &mut ZappyClient, debug: bool) -> Result<(), ClientError> {
    loop {              
        let tiles = client.look().await?;
        if debug {
            println!("Vision actuelle: {:?}", tiles);
        }

        let inventory = client.inventory().await?;
        if debug {
            println!("Inventaire actuel: {:?}", inventory);
        }

        if let Some(first_tile) = tiles.first() {
            if first_tile.contains("food") {
                client.take(Resource::Food).await?;
            } else {
                client.forward().await?;
            }
        }

        let available_slots = client.connect_nbr().await?;
        if debug {
            println!("Slots disponibles: {}", available_slots);
        }

        sleep(Duration::from_millis(100)).await;
    }
}
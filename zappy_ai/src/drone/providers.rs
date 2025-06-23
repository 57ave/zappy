use crate::client::ZappyClient;
use crate::error::ClientError;
use tokio::time::{sleep, Duration};

pub async fn move_to_food(client: &mut ZappyClient, debug: bool) -> Result<(), ClientError> {
    loop {
        let tiles = client.get_look_cached().await?;
      
        if debug {
            println!("Vision actuelle: {:?}", tiles);
        }

        if let Some(first_tile) = tiles.first() {
            if first_tile.contains("food") {
                client.take(super::inventory::Resource::Food).await?;
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
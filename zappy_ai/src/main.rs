mod client;
mod commands;

use clap::Parser;
use tokio::time::{sleep, Duration};

#[derive(Parser)]
#[command(name = "zappy_ai")]
#[command(about = "Zappy AI client")]
struct Args {
    #[arg(short, long, default_value = "4242")]
    port: u16,
    
    #[arg(short, long)]
    name: String,
    
    #[arg(short = 'h', long, default_value = "localhost")]
    machine: String,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    let client = client::ZappyClient::connect(&args.machine, args.port, &args.name, 100).await?;
    Ok(())
}

mod client;
mod commands;

use clap::Parser;
use tokio::time::{sleep, Duration};

#[derive(Parser)]
#[command(name = "zappy_ai")]
#[command(about = "Zappy AI client")]
#[command(help_template = "\n{usage}\n\n{all-args}")]
struct Args {
    #[arg(short, long, default_value = "4242", help = "Port number")]
    port: u16,
    
    #[arg(short, long, help = "Team name")]
    name: String,
    
    #[arg(short = 'm', long, default_value = "localhost", help = "Server hostname")]
    machine: String,

    #[arg(short = 'd', long, help = "Enable debug mode")]
    debug: bool,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    let client = client::ZappyClient::connect(&args.machine, args.port, &args.name, 100, args.debug).await?;
    Ok(())
}

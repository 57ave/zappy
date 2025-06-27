mod client;
mod commands;
mod error;
mod drone;
mod decision;

use clap::Parser;
use error::ClientError;

use crate::decision::decision_caller::make_decision;

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
async fn main() -> Result<(), ClientError> {
    let args = Args::parse();
    let mut client = 
    client::ZappyClient::connect(&args.machine, args.port, &args.name, 100, args.debug).await?;

    make_decision(&mut client).await?;
    Ok(())
}

#!/usr/bin/env bash
set -e

BOLD_GREEN="\e[1;32m"
YELLOW="\e[1;33m"
RESET="\e[0m"

APP_NAME="wikimapper-server"
INSTALL_DIR="/opt/$APP_NAME"
SERVICE_NAME="$APP_NAME.service"
SERVICE_FILE_SRC="./$SERVICE_NAME"
SERVICE_FILE_DST="/etc/systemd/system/$SERVICE_NAME"
ENV_FILE="/etc/default/$APP_NAME"

echo "Building Go binary..."
go build -o $APP_NAME main.go

echo "Installing binary to $INSTALL_DIR..."
sudo mkdir -p $INSTALL_DIR
sudo cp $APP_NAME $INSTALL_DIR/
sudo chown -R www-data:www-data $INSTALL_DIR
sudo chmod +x $INSTALL_DIR/$APP_NAME

if [ -f "$ENV_FILE" ]; then
    echo -e "${YELLOW}Warning: Environment file '$ENV_FILE' already exists. It will NOT be replaced.${RESET}"
    echo -e "${BOLD_GREEN}Current content of $ENV_FILE:${RESET}"
    sudo cat "$ENV_FILE"
    echo "" 
else
    echo -e "${BOLD_GREEN}Creating env file at $ENV_FILE. Remember to set up credentials manually after this script runs.${RESET}"
    sudo tee "$ENV_FILE" > /dev/null <<EOF
NEO4J_URL=http://127.0.0.1:7474
NEO4J_USER=neo4j
NEO4J_PASS=
API_PORT=6348
EOF
    echo -e "${BOLD_GREEN}Environment file created. Please open it to set NEO4J_PASS and API_PORT:${RESET}"
    echo "  sudo vim $ENV_FILE"
fi

echo "Copying $SERVICE_NAME to systemd directory..."
sudo cp $SERVICE_FILE_SRC $SERVICE_FILE_DST

echo "Reloading systemd..."
sudo systemctl daemon-reload

echo "Enabling $APP_NAME to start on boot..."
sudo systemctl enable $APP_NAME

echo "Starting $APP_NAME..."
sudo systemctl restart $APP_NAME

echo "Done! Check status with: sudo systemctl status $APP_NAME"

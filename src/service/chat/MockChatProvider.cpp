#include "MockChatProvider.h"
#include <algorithm>
#include <cctype>

namespace faaliha::faalihamart::service::chat {

namespace {

std::string ToLower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

} // namespace

MockChatProvider::MockChatProvider() {
    faq_database_ = {
        {
            {"hello", "hi", "hey", "greeting", "who are you"},
            "Hello! Welcome to FaalihaMart. I am your AI Marketplace Assistant. I can help you find products across Electronics, Apparel, and Home & Living, track your orders, explain our mock checkout, or assist with seller listings. How may I help you today?"
        },
        {
            {"headphone", "audio", "noise-cancelling", "earphone", "wireless headphones"},
            "We have the 'Noise-Cancelling Wireless Headphones' by Apex Tech Gear for ₹149.99 (40-hour battery life, spatial audio). Check out the Electronics category to add it to your cart!"
        },
        {
            {"keyboard", "gaming keyboard", "mechanical keyboard", "rgb"},
            "The 'Mechanical Gaming Keyboard RGB' is available for ₹89.99 with custom mechanical switches, RGB backlighting, and a detachable braided USB-C cable."
        },
        {
            {"monitor", "screen", "display", "4k"},
            "Check out the 'Ultra-Slim 4K USB-C Monitor 27\"' by Apex Tech Gear for ₹299.99 featuring an IPS panel, 99% sRGB color gamut, and 65W power delivery."
        },
        {
            {"hoodie", "jacket", "apparel", "clothing", "cotton"},
            "Our 'Classic Organic Cotton Hoodie' is available for ₹49.99 tailored from heavyweight 100% sustainable organic cotton. Available in the Apparel category!"
        },
        {
            {"backpack", "bag", "commuter", "laptop bag"},
            "We offer the 'Waterproof Commuter Backpack' for ₹74.99 with weatherproof fabric and a dedicated padded 16-inch laptop compartment."
        },
        {
            {"candle", "tumbler", "home", "living"},
            "In Home & Living, we have the 'Aromatic Soy Wax Candle Set' (₹24.99) and the 'Insulated Stainless Steel Tumbler 32oz' (₹19.99, keeps drinks cold 24h)."
        },
        {
            {"track", "status", "order status", "workflow", "delivery", "where is my order"},
            "FaalihaMart orders follow a strict workflow: PENDING → CONFIRMED → SHIPPED → DELIVERED. You can view real-time status and line items in the 'My Orders' tab."
        },
        {
            {"return", "refund", "exchange", "policy"},
            "FaalihaMart offers a 7-day return policy on any delivered order. Once an order reaches DELIVERED status, you can also leave a verified product review and 1-5 star rating."
        },
        {
            {"payment", "pay", "credit card", "upi", "checkout", "mock payment"},
            "FaalihaMart checkout uses a simulated mock payment authorization step (Mock Credit Card or Mock UPI). No real money is charged during this capstone demonstration."
        },
        {
            {"sell", "seller", "list product", "listing", "vendor"},
            "To sell products, switch to a Seller role in the header dropdown. You will unlock the 'Seller Dashboard' where you can create, update, or remove listings and view sales analytics!"
        },
        {
            {"review", "rating", "star"},
            "Customers who have purchased an item can submit a 1 to 5 star rating and comment after the order has reached the DELIVERED status."
        }
    };
}

std::string MockChatProvider::GetReply(const std::string& user_message,
                                       const std::string& /*context*/) {
    std::string lower_query = ToLower(user_message);

    for (const auto& entry : faq_database_) {
        for (const auto& kw : entry.keywords) {
            if (lower_query.find(kw) != std::string::npos) {
                return entry.answer;
            }
        }
    }

    // Default domain fallback
    return "Thank you for asking! FaalihaMart offers electronics, apparel, and home essentials. You can search products above, manage your cart, track orders (PENDING → CONFIRMED → SHIPPED → DELIVERED), or switch to Seller mode to list items. Is there a specific product or order you'd like help with?";
}

} // namespace faaliha::faalihamart::service::chat

body {
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
    margin: 0;
    padding: 20px;
    background-color: #f4f7f6;
    color: #333;
    line-height: 1.6;
}

header {
    text-align: center;
    margin-bottom: 30px;
    background-color: #fff;
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 2px 5px rgba(0,0,0,0.1);
}

h1 {
    color: #2c3e50;
    font-size: 2.2em;
    margin-bottom: 10px;
}

h2 {
    color: #34495e;
    font-size: 1.8em;
    margin-bottom: 15px;
    border-bottom: 2px solid #ecf0f1;
    padding-bottom: 5px;
}

main {
    display: flex;
    flex-direction: column; /* Stacks lists vertically on small screens */
    gap: 20px;
    max-width: 900px; /* Limits overall width for readability */
    margin: 0 auto; /* Centers the content */
}

.list-container {
    background-color: #fff;
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 2px 5px rgba(0,0,0,0.1);
    flex: 1; /* Allows lists to grow and shrink */
}

.input-section {
    display: flex;
    margin-bottom: 20px;
    gap: 10px; /* Space between input and button */
}

input[type="text"] {
    flex-grow: 1;
    padding: 12px;
    border: 1px solid #ddd;
    border-radius: 5px;
    font-size: 1em;
}

button {
    padding: 12px 20px;
    background-color: #2ecc71; /* Green for add */
    color: white;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 1em;
    transition: background-color 0.2s ease;
}

button:hover {
    background-color: #27ae60;
}

.shopping-list {
    list-style: none;
    padding: 0;
    margin: 0;
}

.shopping-list li {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 12px 0;
    border-bottom: 1px solid #eee;
    font-size: 1.1em;
}

.shopping-list li:last-child {
    border-bottom: none;
}

.shopping-list li span {
    flex-grow: 1; /* Allows item text to take up available space */
}

.shopping-list li .delete-button {
    background-color: #e74c3c; /* Red for delete */
    color: white;
    border: none;
    padding: 8px 12px;
    border-radius: 4px;
    cursor: pointer;
    font-size: 0.9em;
    transition: background-color 0.2s ease;
}

.shopping-list li .delete-button:hover {
    background-color: #c0392b;
}

/* Mobile Optimization */
@media (min-width: 768px) {
    main {
        flex-direction: row; /* Arranges lists side-by-side on larger screens */
    }
}